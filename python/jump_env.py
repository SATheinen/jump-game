import numpy as np
import gymnasium as gym
from gymnasium import spaces
import jumpgame

class JumpGameEnv(gym.Env):
    def __init__(self, config=None):
        super().__init__()
        
        self.game = jumpgame.Game(headless=True)
        self.game.init()
        
        self.action_space = spaces.Discrete(6)
        
        self.observation_space = spaces.Box(
            low=-np.inf,
            high=np.inf,
            shape=(35,),
            dtype=np.float32
        )
        
        self.max_steps = 2000
        self.step_count = 0
        self.last_state = None
        self.last_platform_y = float(0)

    def reset(self, seed=None, options=None):
        super().reset(seed=seed)
        self.game.reset()
        state = self.game.step(False, False, False, 1)
    
        self.step_count = 0
        self.last_state = state.copy()
        self.last_platform_y = float(0)
        return state, {}
    
    def step(self, action):
        left, right, jump = self.action_to_input(action)
        new_state = self.game.step(left, right, jump, 4)
        reward = self._calculate_reward(self.last_state, new_state)
        done = bool(new_state[34] > 0.5)
        
        # Truncation (max steps)
        truncated = False
        self.step_count += 1
        if self.step_count >= self.max_steps:
            truncated = True
        
        # Death penalty
        if done:
            reward -= 50.0
        
        self.last_state = new_state.copy()
        
        info = {
            "score": int(new_state[33]),
            "player_y": float(new_state[1])
        }
        
        return new_state, reward, done, truncated, info

    @staticmethod
    def extract_player(state):
        """Extract player data from state array"""
        return {
            'x': state[0],
            'y': state[1],
            'vx': state[2],
            'vy': state[3],
            'on_ground': bool(state[4])
        }
    
    @staticmethod
    def extract_platforms(state):
        """Extract platform data from state array"""
        platforms = []
        for i in range(7):
            platform = {
                'x': state[5 + i*4],
                'y': state[5 + i*4 + 1],
                'width': state[5 + i*4 + 2],
                'vx': state[5 + i*4 + 3]
            }
            platforms.append(platform)
        return platforms
    
    @staticmethod
    def extract_score(state):
        """Extract score from state array"""
        return int(state[33])

    def action_to_input(self, action):
        "Convert discrete action to (left, right, jump)"
        mapping = {
            0: (False, False, False),
            1: (True, False, False),
            2: (False, True, False),
            3: (False, False, True),
            4: (True, False, True),
            5: (False, True, True),
        }
        return mapping[action]

    def _calculate_reward(self, old_state, new_state):
        reward = 0.0
        
        # Base survival
        reward += 1.0

        height_gain = 0
        height_loss = 0

        # Jump gain
        if old_state[4] == 1 and new_state[4] == 0:
            reward += 1.0
        
        # Landing on platform
        if old_state[4] == 0 and new_state[4] == 1:  # Just landed!
            current_platform_y = new_state[1]  # Player Y = Plattform Y
            
            # Vergleiche mit letzter Plattform
            if current_platform_y < self.last_platform_y:
                # HÖHER! (negativere Y = höher)
                reward += 20.0
            elif current_platform_y > self.last_platform_y:
                reward -= 5.0
            
            # UPDATE: Merke diese Plattform!
            self.last_platform_y = current_platform_y
        
        # Kleine guidance für Bewegung
        height_change = -(new_state[1] - old_state[1])
        reward += height_change * 0.05

        # penalty wenn der score zu langsam steigt
        reward -= 0.1 * self.step_count / (1.0 + new_state[33])

        if self.step_count % 100 == 0:
            print(f"Step {self.step_count}: height gain={height_gain:.2f}, height loss={height_loss:.0f}, current platform={self.last_platform_y}")
        
        return reward
    
    def close(self):
        self.game.clean()
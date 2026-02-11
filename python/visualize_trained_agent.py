import time
import ray
from ray.rllib.algorithms.ppo import PPO
from ray.tune import register_env
import jumpgame
from jump_env import JumpGameEnv
import numpy as np
import torch
import warnings
warnings.filterwarnings("ignore", category=DeprecationWarning)

register_env("jump_game", lambda config: JumpGameEnv(config))

checkpoint = "/var/folders/31/lhm9rkfd5mq5nqs778m806v40000gn/T/tmph5cfdkml"
agent = PPO.from_checkpoint(checkpoint)

rl_module = agent.get_module()

def action_to_input(action):
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

game = jumpgame.Game(headless=False)
game.init()

done = False
num_steps = 2000

for step in range(num_steps):
    state = game.getState()

    obs_batch = torch.Tensor(state)

    output = rl_module.forward_inference({"obs": obs_batch})

    action_logits = output["action_dist_inputs"]
    
    action = torch.argmax(action_logits[0])  # Greedy action
    left, right, jump = action_to_input(int(action))

    game.runAgentVisualization(left, right, jump)

    done = state[34] > 0.5

    if done == True:
        print("Player Died")
        break

game.clean()
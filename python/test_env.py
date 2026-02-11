from jump_env import JumpGameEnv
import numpy as np

# Create environment
env = JumpGameEnv()

print("Testing JumpGameEnv...")
print(f"Action space: {env.action_space}")
print(f"Observation space: {env.observation_space}")

# Test reset
obs, info = env.reset()
print(f"\nInitial observation shape: {obs.shape}")
print(f"Initial player pos: ({obs[0]}, {obs[1]})")

# Test random actions
total_reward = 0
for i in range(100):
    action = env.action_space.sample()  # Random action
    obs, reward, done, truncated, info = env.step(action)
    total_reward += reward
    
    if i % 20 == 0:
        print(f"Step {i}: y={obs[1]:.1f}, reward={reward:.3f}, total={total_reward:.2f}")
    
    if done or truncated:
        print(f"Episode ended at step {i}")
        break

env.close()
print("\nEnvironment test complete!")
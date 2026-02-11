import ray
from ray.rllib.algorithms.ppo import PPOConfig
from ray.tune.registry import register_env
from jump_env import JumpGameEnv
import os
import warnings
warnings.filterwarnings("ignore", category=DeprecationWarning)

# Suppress GPU warning
os.environ["RAY_ACCEL_ENV_VAR_OVERRIDE_ON_ZERO"] = "0"

def env_creator(env_config):
    """Create environment for RLlib"""
    return JumpGameEnv(config=env_config)

# Register environment
register_env("jump_game", env_creator)

# Initialize Ray
ray.init(ignore_reinit_error=True)

# Configure PPO (Updated for Ray 2.9.0+)
config = (
    PPOConfig()
    .environment("jump_game")
    .framework("torch")
    .env_runners(
        num_env_runners=4,
        rollout_fragment_length=200,
    )
    .training(
        train_batch_size=4000,
        minibatch_size=128,
        num_sgd_iter=10,
        lr=3e-4,
        gamma=0.99,
        lambda_=0.95,
        clip_param=0.2,
        entropy_coeff=0.1,
    )
    .resources(
        num_gpus=1,
    )
    .debugging(
        log_level="INFO"
    )
)

# Build algorithm
algo = config.build()

print("Starting training...")
print("=" * 60)

# Training loop - DEBUG VERSION
num_iterations = 100
best_reward = float('-inf')

for i in range(num_iterations):
    result = algo.train()
    
    # Try different possible locations for metrics
    mean_reward = None
    episode_len = None
    
    # Option 1: env_runners (Ray 2.9+)
    if "env_runners" in result:
        mean_reward = result["env_runners"].get("episode_reward_mean")
        episode_len = result["env_runners"].get("episode_len_mean")
    
    # Option 2: Direct (Ray 2.0-2.8)
    if mean_reward is None:
        mean_reward = result.get("episode_reward_mean")
        episode_len = result.get("episode_len_mean")
    
    # Option 3: sampler_results (some versions)
    if mean_reward is None and "sampler_results" in result:
        mean_reward = result["sampler_results"].get("episode_reward_mean")
        episode_len = result["sampler_results"].get("episode_len_mean")
    
    # If still not found, skip this iteration
    if mean_reward is None:
        print(f"Iteration {i+1}: Metrics not available yet, waiting...")
        continue
    
    # Print progress
    print(f"Iteration {i+1}/{num_iterations}")
    print(f"  Mean Reward: {mean_reward:.2f}")
    print(f"  Episode Length: {episode_len:.1f}")
    print(f"  Time: {result['time_total_s']:.1f}s")
    print("-" * 60)
    
    # Save best model
    if mean_reward > best_reward:
        best_reward = mean_reward
        checkpoint_dir = algo.save()
        print(f"  ✓ New best! Saved checkpoint to: {checkpoint_dir}")
        print("-" * 60)

print("\nTraining complete!")
print(f"Best mean reward: {best_reward:.2f}")
print()

# Final save
final_checkpoint = algo.save()
print(f"Final checkpoint saved to: {final_checkpoint}")

# Cleanup
algo.stop()
ray.shutdown()
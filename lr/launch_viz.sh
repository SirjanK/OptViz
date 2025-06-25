# default viz configuration for simple LR
# assume you are running this from project root

python lr/viz.py \
    --params_path "lr/assets/params.csv" \
    --save_path "lr/assets/animation.mp4" \
    --n_samples 1000 \
    --fps 60 \
    --frame_step 50

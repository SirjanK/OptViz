# default training configuration for simple LR
# assume you are running this from project root

python lr/train.py \
    --n_train 10000 \
    --n_val 1000 \
    --batch_size 32 \
    --epochs 100 \
    --lr 0.001 \
    --val_interval 10 \
    --log_dir "lr/logs" \
    --params_path "lr/assets/params.csv"

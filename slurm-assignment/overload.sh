#!/bin/bash
#SBATCH -J overload
#SBATCH -N 1
#SBATCH -n 1
#SBATCH -t 00:02:00
#SBATCH -o overload_%j.out

echo "Job ID: $SLURM_JOB_ID"
echo "Requested: 1 CPUs"
echo "Running stress on 1 cores"
echo "Started at $(date)"

stress --cpu 1 --timeout 60s

echo "Finished at $(date)"
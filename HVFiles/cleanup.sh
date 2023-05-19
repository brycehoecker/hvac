#!/bin/bash

DIR=Reduced_HV
for X in {1..4}; do
    new=$(ls -1t $DIR/T${X}_* | head -1)
    find $DIR -name "T${X}_*" ! -newer $new ! -samefile $new -exec mv '{}' OLD \;
done

#!/bin/bash

while read line
do
  ./Snapshot $line
done < $1

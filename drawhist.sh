#!/bin/bash

var=${1}
cut=${2}
nbins=${3:-100}
xlow=${4:-0}
xhigh=${5:-100}
isLogy=${6:-0}
saveCanv=${7:-0}
text=${8:-""}

root -l varcutplot.C\(\"${var}\",\"${cut}\",${nbins},${xlow},${xhigh},${isLogy},${saveCanv},\"${text}\"\)

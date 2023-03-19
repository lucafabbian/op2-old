#!/bin/bash
cd $(dirname "$BASH_SOURCE[0]")

rm -rf ./report
mkdir -p ./report
cp TSP.ipynb ./report
cd ./report
jupyter nbconvert TSP.ipynb --to latex --output TSP.tex --embed-images --LatexPreprocessor.style=vs

# compile the report from latex to pdf
pdflatex TSP.tex
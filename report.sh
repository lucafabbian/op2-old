#!/bin/bash
cd $(dirname "$BASH_SOURCE[0]")
export TITLE="The Travelling Salesman Problem"

rm -rf ./report
mkdir -p ./report
cp TSP.ipynb "./report/$TITLE.ipynb"
cd ./report
jupyter nbconvert "$TITLE.ipynb" --to latex --output TSP.tex --embed-images --LatexPreprocessor.style=vs

# compile the report from latex to pdf
pdflatex TSP.tex
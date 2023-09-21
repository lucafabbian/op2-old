#!/bin/bash
cd $(dirname "$BASH_SOURCE[0]")
export TITLE="The Travelling Salesman Problem"

rm -rf ./report
mkdir -p ./report

cp TSP.ipynb ./report
cp refs.bib ./report
cp ieee.csl ./report
cd ./report
jupyter nbconvert --to markdown TSP.ipynb
pandoc -s TSP.md -t pdf -o TSP.pdf \
     --pdf-engine-opt="-interaction=nonstopmode" \
   --filter pandoc-citeproc --bibliography="refs.bib" --csl="ieee.csl" -N -V title:"$TITLE"

#cp TSP.ipynb "./report/$TITLE.ipynb"
#cp refs.bib  ./report
#cd ./report
#jupyter nbconvert "$TITLE.ipynb" --to latex --output TSP.tex --embed-images --LatexPreprocessor.style=vs
#
## compile the report from latex to pdf
#pdflatex -interaction=nonstopmode TSP.tex
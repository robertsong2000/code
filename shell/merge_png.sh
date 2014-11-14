#!/bin/sh
count=0;
for i in *.png; do
    echo $i;
    count=$(($count + 1));
    mudraw -F pdf -o $count.pdf "$i";
done
gs -q -dNOPAUSE -sDEVICE=pdfwrite -sOUTPUTFILE=final.pdf -dBATCH *.pdf

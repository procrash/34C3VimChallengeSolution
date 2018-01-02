cp dotFile.txt dotFile2.txt
echo "}" >>dotFile2.txt
dot -Tpng dotFile2.txt > output.png
open output.png

echo "}" >>dotFile.txt
dot -Tpng dotFile.txt > output.png
open output.png

echo "Folder filer copy"
for ((fileC=$2+1; fileC<=$3;fileC++))
do
	`cp -r ../RootFiles/Event/Run_$1/file$2/ ../RootFiles/Event/Run_$1/file$fileC`
done
echo "done"

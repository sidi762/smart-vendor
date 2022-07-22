i=1
dir='15_empty'
mkdir ../val/$dir/
mkdir ../test/$dir/
while ((i <= 400))
do
	file=`/bin/ls -1 "$dir" | sort --random-sort | head -1`
	path=`greadlink --canonicalize "$dir/$file"` # Converts to full path
	echo "The randomly-selected file for val is: $path"
	mv $path ../val/$dir/$file
	((i++))
done
i=1
while ((i <= 400))
do
        file=`/bin/ls -1 "$dir" | sort --random-sort | head -1`
        path=`greadlink --canonicalize "$dir/$file"` # Converts to full path
        echo "The randomly-selected file for test is: $path"
        mv $path ../test/$dir/$file
        ((i++))
done

#!/bin/bash

if [ $# -ne 2 ]; then
  echo "Please, enter all parameters $0 <dir_path> <regex>"
  exit 1
fi

path=$1
pattern=$2

if [ ! -d "$path" ]; then
  echo "No such file or directory."
  exit 1
fi


traverse_directory() {
  local dir="$1"
  for file in "$dir"/*; do
    if [ -f "$file" ]; then

      if [[ $(basename "$file") =~ $pattern ]]; then
        echo "$file founded"
        echo "$(basename "$file")" >> output.txt
      fi
    elif [ -d "$file" ]; then
      # if [ "$file" != "$dir" ]; then
      traverse_directory "$file"
      # fi
    fi
  done
}

if [ -e "output.txt" ]; then
  > output.txt
fi

traverse_directory "$path"

echo "Done! You can check results in output.txt."

exit 0
#!/bin/bash

usage() {
  echo "Usage: $0 [-r <regex>|-n <filename>] <dir_path>"
  exit 1
}

if [ $# -lt 2 ]; then
  usage
fi

pattern=""
filename=""
path=""

while getopts ":r:n:" opt; do
  case $opt in
    r)
      pattern="$OPTARG"
      ;;
    n)
      filename="$OPTARG"
      ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2
      usage
      ;;
  esac
done

shift $((OPTIND - 1))
path="$1"

if [ -z "$pattern" ] && [ -z "$filename" ]; then
  echo "Please specify either a regex pattern (-r) or a filename (-n)."
  usage
fi

if [ ! -d "$path" ]; then
  echo "No such file or directory."
  exit 1
fi

if [ -e "output.txt" ]; then
  > output.txt
fi

calculate_checksum() {
    local file="$1"
    
    checksum=$(md5sum "$1" | awk '{print $1}')

    echo "$checksum"
}

traverse_directory() {
  local dir="$1"
  for file in "$dir"/*; do
    if [ -f "$file" ]; then
      if [ -n "$pattern" ]; then
        if [[ $(basename "$file") =~ $pattern ]]; then
          checksum=$(calculate_checksum "$file")
          echo "$file founded, checksum: $checksum"
          echo "$(basename "$file, checksum: $checksum")" >> output.txt
        fi
      elif [ -n "$filename" ]; then
        if [ "$(basename "$file")" = "$filename" ]; then
          checksum=$(calculate_checksum "$file")
          echo "$file founded, checksum: $checksum"
          echo "$(basename "$file, checksum: $checksum")" >> output.txt
        fi
      fi
    elif [ -d "$file" ]; then
      traverse_directory "$file"
    fi
  done
}

traverse_directory "$path"

echo "Search completed! You can check results in output.txt."

echo "Enter file name, which you want to cat: "

read filename

if [ -f "$filename" ]; then
    clear
    lineNumber=1
    while IFS= read -r line; do
        printf "%-5s %s\n" "$lineNumber:" "$line"
        lineNumber=$((lineNumber+1))
    done < "$filename"
else
    echo "$filename: no such file or directory."
fi

exit 0
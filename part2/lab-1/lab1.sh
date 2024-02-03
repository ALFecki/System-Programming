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
    local sum=0

    while IFS= read -r -n1 byte; do
        byte_value=$(printf "%d" "'$byte")
        ((sum += byte_value))
    done < "$file"

    echo "$sum"
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
      # if [ "$file" != "$dir" ]; then
      traverse_directory "$file"
      # fi
    fi
  done
}



traverse_directory "$path"

echo "Done! You can check results in output.txt."

exit 0
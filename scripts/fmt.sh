#!/bin/bash
fmt() {
  clang-format src/*.{h,hpp} src/**/*.hpp "$@"
}

if [[ $1 == "check" ]]; then
  fmt -output-replacements-xml | grep -c "<replacement " >/dev/null
	if [[ $? -ne 1 ]]; then 
		echo "Not all files match formatting"
		exit 1
	fi
else
  fmt -i
fi

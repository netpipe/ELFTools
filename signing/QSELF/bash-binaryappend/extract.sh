#!/bin/bash

uuencode=1
binary=0

function untar_payload()
{
	match=$(grep --text --line-number '^PAYLOAD:$' $0 | cut -d ':' -f 1)
	payload_start=$((match + 1))
	if [[ $binary -ne 0 ]]; then
		tail -n +$payload_start $0 | tar -tzvf -
	fi
	if [[ $uuencode -ne 0 ]]; then
		tail -n +$payload_start $0 | uudecode | tar -tzvf -
	fi
}

read -p "Install files? " ans
if [[ "${ans:0:1}"  ||  "${ans:0:1}" ]]; then
	untar_payload
	# Do remainder of install steps.
fi

exit 0


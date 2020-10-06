#!/bin/bash

set -x

assets=()
for asset in RPMS/*.rpm
do
  assets+=("-a" "$asset")
done

tag_name="${GITHUB_REF##*/}"
tag_name="${tag_name/%-sfos*}"

if [[ "`hub release show $tag_name -f %T`" = "$tag_name" ]]
then
  hub release edit "${assets[@]}" -m "" "$tag_name"
else
  hub release create "${assets[@]}" -m "$tag_name" "$tag_name"
fi

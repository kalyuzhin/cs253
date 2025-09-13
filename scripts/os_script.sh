#!/bin/zsh

platform='unknown'
unamestr=$(uname 2>/dev/null) # если команда недоступна, то ошибка не выпадет

if [[ "$unamestr" == 'Linux' ]]; then
  platform='linux'
elif [[ "$unamestr" == 'Darwin' ]]; then
  platform='macOS'
elif [["$OS" == 'Windows_NT']]; then
  platform='windows'
fi

echo "$platform"
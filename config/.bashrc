# .bashrc

# Source global definitions
if [ -f /etc/bashrc ]; then
	. /etc/bashrc
fi

# User specific aliases and functions
alias vi="vimx"
stty -ixon

# From http://jeroenjanssens.com/2013/08/16/quickly-navigate-your-filesystem-from-the-command-line.html
export MARKPATH=$HOME/.marks
function jump {
    cd -P $MARKPATH/$1 2> /dev/null || echo "No such mark: $1"
}
function mark {
    mkdir -p $MARKPATH; ln -s $(pwd) $MARKPATH/$1
}
function unmark {
    rm -i $MARKPATH/$1
}
function marks {
    ls -l $MARKPATH | sed 's/  / /g' | cut -d' ' -f9- && echo
}
function _jump {
      local cur=${COMP_WORDS[COMP_CWORD]}
      local marks=$(find $MARKPATH -type l | awk -F '/' '{print $NF}')
      COMPREPLY=($(compgen -W '${marks[@]}' -- "$cur"))
      return 0
}
complete -o default -o nospace -F _jump jump

alias pd=pushd
alias pop=popd
alias w3m="w3m -B"

function ff {
	find . -iname "*$1*"
}
PATH=$PATH:$HOME/arm-2014.05/bin

export HISTSIZE=1000000
export HISTFILESIZE=1000000 
alias h="history"

function make-kconfig-cscope {
	find . -name "Kconfig*" -not -wholename "*\.pc*" > kconfig.cscope.files
	cscope -bi kconfig.cscope.files -f kconfig.cscope.out
}

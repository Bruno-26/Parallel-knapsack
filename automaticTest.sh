#!/bin/bash
GREY="\033[01;30m"
RED="\033[01;31m"
GREEN="\033[01;32m"
YELLOW="\033[01;33m"
BLUE="\033[01;34m"
PURPLE="\033[01;35m"
CYAN="\033[01;36m"
WHITE="\033[01;37m"
CLOSE="\033[m"
NULL="/dev/null"


INPUTS=('500' '1000' '5000' '10000')
ANSWERS=('7117' '14390' '72505' '146919')
EXEC=2
CORES=6


ok() {
    echo -ne "${RED}[${CLOSE}${WHITE}$(date +%d/%m/%y) - $(date +%H:%M:%S)${CLOSE}${RED}]${CLOSE} ${WHITE}>${CLOSE}"
    echo -e "${GREEN} ${1} ${CLOSE}" >&2
}

erro() {
    echo -ne "${RED}[${CLOSE}${WHITE}$(date +%d/%m/%y) - $(date +%H:%M:%S)${CLOSE}${RED}]${CLOSE} ${WHITE}>${CLOSE}"
    echo -e "${RED} ${1} ${CLOSE}" >&2
}

info() {
    echo -ne "${RED}[${CLOSE}${WHITE}$(date +%d/%m/%y) - $(date +%H:%M:%S)${CLOSE}${RED}]${CLOSE} ${WHITE}>${CLOSE}"
    echo -e "${BLUE} ${1} ${CLOSE}" >&2
}

info2() {
    echo -ne "${RED}[${CLOSE}${WHITE}$(date +%d/%m/%y) - $(date +%H:%M:%S)${CLOSE}${RED}]${CLOSE} ${WHITE}>${CLOSE}"
    echo -e "${PURPLE} ${1} ${CLOSE}" >&2
}

loading() {
    echo -ne "\r${RED}[${CLOSE}${WHITE}$(date +%d/%m/%y) - $(date +%H:%M:%S)${CLOSE}${RED}]${CLOSE} ${WHITE}>${CLOSE}"
    echo -ne "${CYAN} ${1} ${CLOSE}" >&2
}

testResults() {
    CORE="${1}"

    for ((cont=0; cont<${#INPUTS[@]}; cont++)); do
        grep -q '[^[:space:]]' < "resultados/core$CORE/results_${INPUTS[$cont]}.txt"
        blank=$?
        cat resultados/core$CORE/results_${INPUTS[$cont]}.txt | grep Result | cut -f2 -d":" | tr -d " " | grep -v ${ANSWERS[$cont]} &> $NULL
        temp=$?
        if [[ $temp -eq 0 || $blank -eq 1 ]]; then
            erro "Teste: $N \tCore: $CORE \t✕"
        else
            ok "Teste: $N \tCore: $CORE \t✓"
        fi
    done
    ok "---------------------------------"
}

calculo() {
    CORE="${1}"
    info2 "Cores: $CORE"

    for ((cont=0; cont<${#INPUTS[@]}; cont++)); do
        info "-> ${INPUTS[$cont]}"
        for num in $(seq 1 $EXEC); do
						mpirun -np $CORE ./a.out < inputs/${INPUTS[$cont]} >> resultados/core$CORE/results_${INPUTS[$cont]}.txt 2> $NULL
            calc="$(expr 100 \* $num / $EXEC)"
            loading "$calc%"
        done
        echo -ne "\r"
    done
}

timeFilter() {
    CORE="${1}"
    for ((cont=0; cont<${#INPUTS[@]}; cont++)); do
        cat resultados/core$CORE/results_${INPUTS[$cont]}.txt | grep Time | cut -f2 -d":" | tr -d " " | tr -d "s" | tr "." "," > resultados/core$CORE/temp_${INPUTS[$cont]}.txt
    done
}

ctrlc () {
    echo
    erro "Script Abortado"
    exit 2
}

trap "ctrlc" 2

rm -rf resultados
rm -rf a.out

mkdir resultados

mpiCC -O3 knapSack.c

for num in $(seq 1 $CORES);do
    mkdir resultados/core$num
done

clear
ok "Pastas criadas"

for num in $(seq 1 $CORES);do
    calculo $num
done

info2 "Testando Resultados:"
for num in $(seq 1 $CORES);do
    testResults $num
done

info "Criando Arquivos que contenham apenas o tempo 'temp*'"
for num in $(seq 1 $CORES);do
    timeFilter $num
done

rm -rf a.out
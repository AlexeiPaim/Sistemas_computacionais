#!/bin/bash

#Trabalho pratico 1
#	Aléxei Felipe Paim (20250264)
#	Fabricio Fabricio Zimmermann (19150429)


#endereço IP que sera usado
ID1="127.0.0.1"

# Loop para Abrir workes
for i in {1..6}
do
  ./worker "$ID1" &
  sleep 0.2
done

# Recebe o valor de chamadaspor segundo
echo -n "Quantas vezes por segundo:"
read client_s

# Executa as chamadas aleatorios por tempo indeterminado 
while true; do

    # Gera uma operação aleatória para ID2
    OPERATIONS=("add" "subtract" "multiply" "divide")
    ID2=${OPERATIONS[$RANDOM % ${#OPERATIONS[@]}]}

    # Gera números aleatórios para ID3 e ID4
    ID3=$(shuf -i 1-10 -n 1)
    ID4=$(shuf -i 1-10 -n 1)

    # Executa o comando com os valores gerados
    ./client "$ID1" "$ID2" "$ID3" "$ID4"  &
    
    # Calcula o tempo entre as chamadas 
    intervalo=$(echo "scale=6; 1 / $client_s" | bc)
    sleep $intervalo
done


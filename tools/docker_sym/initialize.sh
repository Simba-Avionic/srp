#!/bin/bash

# Nazwa sieci, którą chcemy sprawdzić lub utworzyć
NETWORK_NAME="simba_network"

# Funkcja do tworzenia sieci, jeśli nie istnieje
create_network() {
    if [ "$(sudo docker network ls -q -f name=^${NETWORK_NAME}$)" ]; then
        echo "Sieć '${NETWORK_NAME}' już istnieje."
    else
        # Utwórz sieć
        sudo docker network create --subnet=192.168.10.0/24 ${NETWORK_NAME}
        echo "Sieć '${NETWORK_NAME}' została utworzona."
    fi
}

# Funkcja do usuwania kontenera, jeśli istnieje
delete_container() {
    CONTAINER_NAME=$1
    if [ -z "${CONTAINER_NAME}" ]; then
        echo "Użycie: delete_container <nazwa_kontenera>"
        return 1
    fi

    if [ "$(sudo docker ps -a -q -f name=^${CONTAINER_NAME}$)" ]; then
        # Zatrzymaj kontener
        sudo docker stop ${CONTAINER_NAME}
        # Usuń kontener
        sudo docker rm ${CONTAINER_NAME}
        echo "Kontener '${CONTAINER_NAME}' został zatrzymany i usunięty."
    else
        echo "Kontener '${CONTAINER_NAME}' nie istnieje."
    fi
}

# Sprawdzenie, czy podano odpowiednią liczbę argumentów
if [ "$#" -ne 2 ]; then
    echo "Użycie: $0 <nazwa_kontenera> <adres_ip>"
    exit 1
fi

# Przypisanie argumentów do zmiennych
CONTAINER_NAME=$1
IP_ADDRESS=$2

# Tworzymy sieć, jeśli nie istnieje
create_network

# Usuwamy kontener, jeśli uruchomiony
delete_container $CONTAINER_NAME

# Uruchamiamy kontener
echo "Uruchamianie kontenera '${CONTAINER_NAME}' z adresem IP ${IP_ADDRESS}..."
sudo docker container run -d --name $CONTAINER_NAME --network ${NETWORK_NAME} --ip ${IP_ADDRESS} matiko42/simba_sim_container:latest
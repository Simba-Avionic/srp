if [ ! -d "/etc/simba" ]; then
        cd /etc
        mkdir simba
        echo "SRP sw dir  added"
fi

if [ ! -d "/etc/simba/backup" ]; then
        cd /etc/simba
        mkdir backup
        echo "Backup dir added"
fi

if [ ! -d "/etc/simba/new" ]; then
        cd /etc/simba
        mkdir new
        echo "New dir added"
fi

if [ ! -d "/etc/simba/current" ]; then
        cd /etc/simba
        mkdir current
        echo "Current dir added"
fi

if [ "$(ls -A /etc/simba/new | grep .tar)" ]; then
    echo "SRP update detected:"
    ls -A /etc/simba/new

    if [ ! -d "/opt/cpu_simba/update.sh" ]; then
        echo "Update script not detected !!!"
        for file in /etc/simba/new/*.tar; do
            echo "Opening: $file"
            cp $file /etc/simba/current
            tar -xvf $file -C /
            rm $file
        done
    else
        sh /opt/cpu_simba/update.sh
    fi
else
    echo "SRP update not detected"
fi

sh /opt/cpu_simba/component_start_up.sh
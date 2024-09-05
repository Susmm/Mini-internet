#!/bin/bash

# Check if argument provided
if [[ $# -lt 2 ]] || [[ "$1" == "up" && $# -ne 3 ]]; then
    echo "Usage: $0 <up/down> <interface> <up ? domain_file_path>"
    exit 1
fi


# Network interface name
interface="$2"

# Determine action based on argument
action="$1"

# Function to increment IP address
increment_ip() {
	local ip="$1"
	local octets
	IFS='.' read -r -a octets <<< "$ip"
	local carry=1

	for ((i=${#octets[@]} - 1; i >= 0; i--)); do
		octets[i]=$(( ${octets[i]} + carry ))
		if (( ${octets[i]} > 255 )); then
		    octets[i]=0
		    carry=1
		else
		    carry=0
		    break
		fi
	done

	echo "${octets[*]}" | tr ' ' '.'
}

create_subinterfaces() {
	# Get the starting IP address of the specified interface
	starting_ip=$(ip addr show dev "$interface" | awk '/inet / {print $2}' | cut -d'/' -f1 | head -n 1)

	# Check if starting IP address retrieved successfully
	if [ -z "$starting_ip" ]; then
		echo "Failed to retrieve starting IP address for interface $interface"
		exit 1
	fi

	echo "Starting IP address for interface $interface: $starting_ip"

	domain_file="$1"

	# Check if the file exists
	if [ ! -f "$domain_file" ]; then
		echo "File '$domain_file' not found"
		exit 1
	fi

	# Netmask
	netmask="255.255.255.0"

	ip="$starting_ip"
	i=0

	command_to_execute=""
	
	while read -r line; do
		    domain_name=$(echo "$line" | awk '{print $1}')  # Extract domain name from each line
		    subinterface="$interface:$i"         # Subinterface name
		    ip_addr="$ip"                                  # IP address for subinterface

		    if [ "$ip" != "$starting_ip" ]; then
		    	echo "Creating subinterface $subinterface with IP address $ip_addr and netmask $netmask"
		    	command_to_execute+="ifconfig $subinterface $ip_addr netmask $netmask up; "
		    fi
		    # Update file with IP address beside domain name
		    sed -i "s/^$domain_name.*/$domain_name $ip_addr/" "$domain_file"
		    
		    # Create a file with the IP address as the filename if it doesn't already exist
		    if [ ! -e "$ip_addr" ]; then
		        touch "$ip_addr"
		    fi
		    # Increment IP address for the next subinterface
		    ip=$(increment_ip "$ip")
		    
		    ((i++))
		    
		done < "$domain_file"
		
	# Create subinterfaces using a single sudo invocation
	sudo bash -c "$command_to_execute"
}

destroy_subinterfaces() {
	ip addr show | grep -E "$interface:[0-9]+" | awk '{print $NF}' |
    while read -r subinterface; do
        # Bring down the subinterface
        #sudo ip link set dev "$subinterface" down
        sudo ifconfig "$subinterface" down
        echo "Destroying subinterface $subinterface"
    done

	echo "All subinterfaces for interface $interface are destroyed."
}

if [ "$action" == "up" ]; then
	create_subinterfaces "$3"
else
	destroy_subinterfaces
fi

echo ""
ip address show $interface


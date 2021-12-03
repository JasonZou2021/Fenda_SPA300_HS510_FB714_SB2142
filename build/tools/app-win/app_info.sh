
check4byte() {
	if ! [ "$1" -eq "$1" 2>/dev/null ]; then
		if [[ $1 =~ ^0x[0-9A-Fa-f]{1,}$ ]] ; then
			if [ "$(($1))" -gt "$((0xffffffff))" ]; then
				return 1
			fi
			return 0
		fi
		return 1
	elif [ "$1" -gt "$((0xffffffff))" ]; then
		return 1
	fi
	return 0
}
check2byte() {
	if ! [ "$1" -eq "$1" 2>/dev/null ]; then
		if [[ $1 =~ ^0x[0-9A-Fa-f]{1,}$ ]] ; then
			if [ "$(($1))" -gt "$((0xffff))" ]; then
				return 1
			fi
			return 0
		fi
		return 1
	elif [ "$1" -gt "$((0xffff))" ]; then
		return 1
	fi
	return 0
}

# 23bytes (String)
VENDOR_NAME() {
	if [ "${#1}" -ge "24" ]; then
		return 1
	fi
	return 0
}

# 4bytes (int)
PRODUCT_ID() {
	(check4byte "$1")
	return $?
}
# 2bytes (short)
MAIN_VERSION() {
	(check2byte "$1")
	return $?
}
# 2bytes (short)
SUB_VERSION() {
	(check2byte "$1")
	return $?
}
# 4bytes (int)
MODE() {
	(check4byte "$1")
	return $?
}

while getopts "c:" opt
do
	case "$opt" in
		c) config_file="$OPTARG" ;;
	esac
done

cmd=("DONE" "VENDOR_NAME" "PRODUCT_ID" "MAIN_VERSION" "SUB_VERSION" "MODE")

for ((i=1; i<${#cmd[@]}; i++)); do
	tmp=$(awk -F= '/^'${cmd[$i]}'/ {gsub(/^ */,"",$2); gsub(/ *$/,"",$2); print $2}' $config_file)
	eval "vvv_${cmd[$i]}=\$tmp"
done

while true; do
	echo
	echo
	echo "---------------------------------------"
	echo "           Config Setting"
	echo "---------------------------------------"

	for ((i=1; i<${#cmd[@]}; i++)); do
		tmp="vvv_${cmd[$i]}"
		printf "%3d. %s = %s\n" $i "${cmd[$i]}" "${!tmp}"
	done

	read -p "Select [1 .. $((${#cmd[@]}-1)), 0 = Write File & exit] : " select_num

	if ! [ "$select_num" -eq "$select_num" 2>/dev/null ]; then
		continue
	elif [ "$select_num" -le "${#cmd[@]}" ] && [ "$select_num" -ge "1" ]; then
		while true; do
			echo "    Tips: <Null for Skip>"
			read -p "    Set ${cmd[$select_num]} = " tmp
			if [ -z "$tmp" ]; then
				break;
			else
				(${cmd[$select_num]} "$tmp")
				if [ "$?" -eq "0" ]; then
					eval "vvv_${cmd[$select_num]}=\$tmp"
					break;
				else
					echo "    Input Error!"
					echo
				fi
			fi
		done
	elif [ "$select_num" -eq "0" ]; then
		break;
	fi
done

echo
echo
echo "---------------------------------------"
echo "            Final Config"
echo "---------------------------------------"
for ((i=1; i<${#cmd[@]}; i++)); do
	tmp="vvv_${cmd[$i]}"
	printf "%s = %s\n" "${cmd[$i]}" "${!tmp}"
	sed -i "s/^${cmd[$i]}.*=.*/${cmd[$i]} = ${!tmp}/" $config_file
done


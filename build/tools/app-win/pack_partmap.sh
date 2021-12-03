
# xb_en=0 is all not xb
# xb_en=1 is all xb
# xb_en=2 is xb part data, map file not xb
xb_en=0
# custom_mode=0 is disable custom mode rom.bin
# custom_mode=1 for quickly generator spupdate_app.bin
# custom_mode=2 for quickly generator spupdate_all.bin
# custom_mode=3 for all custom spupdate.bin
custom_mode=0
# debug log
show_log=0
# auto app.bin & app_b.bin select default on
disable_auto_select_app=0

while getopts "ldc:x:p:i:b:s:g:o:a:" opt
do
	case "$opt" in
		l) show_log=1 ;;
		d) disable_auto_select_app=1 ;;
		c) custom_mode="$OPTARG" ;;
		x) xb_en="$OPTARG" ;;
		p) ispbin_file="$OPTARG" ;;
		i) include_file="$OPTARG" ;;
		b) flash_sector_size="$OPTARG" ;;
		s) start_addr="$OPTARG" ;;
		g) get_offset_bybin="$OPTARG" ;;
		o) output_file="$OPTARG" ;;
		a) part_args="$OPTARG" ;;
	esac
done

. $include_file

custom_allpart () {
	set -- $partition_tbl
	sel_part_tbl=
	pos=1
	part_app_bin_idx=
	part_appb_bin_idx=
	while [ ! -z $1 ]; do
		if [ "$(basename $1)" = "app_b.bin" ]; then
			if [ ! -f $1 ]; then
				shift 2
				continue
			else
				part_appb_bin_idx=$pos
			fi
		elif [ "$(basename $1)" = "app.bin" ]; then
			part_app_bin_idx=$pos
		fi

		sel_part_tbl="$sel_part_tbl $(basename $1)"
		pos=$(($pos+1))
		shift 2
	done

	sel_part_tbl=($sel_part_tbl)
	sel_part_arr=

	part_args_arr=($part_args)
	part_args_idx=0

	while true; do
		if [ "${#part_args_arr[@]}" -eq "0" ]; then
			echo
			echo
			echo "---------------------------------------"
			echo "           Input Partitions"
			echo "---------------------------------------"

			i=1
			for name in ${sel_part_tbl[@]}; do
				printf "%3d. %s\n" $i $name
				i=$(($i+1))
			done

			read -p "Input Partitions [1 .. ${#sel_part_tbl[@]}, eg: 1 3 4] : " select_num
			part_args_arr=($select_num)
		fi

		if ! [ "${part_args_arr[$part_args_idx]}" -eq "${part_args_arr[$part_args_idx]}" 2>/dev/null ]; then
			select_num=-1
			set -- $partition_tbl
			i=1
			while [ ! -z $1 ]; do
				if [ "${part_args_arr[$part_args_idx]}" == "$(basename $1)" ]; then
					select_num=$i
					break
				fi
				i=$(($i+1))
				shift 2
			done
		else
			select_num=${part_args_arr[$part_args_idx]}
		fi

		if [ "$select_num" -le "0" ] || [ "$select_num" -gt "${#sel_part_tbl[@]}" ]; then
			echo "Input Error Partition name: \"${part_args_arr[$part_args_idx]}\""
		fi

		part_args_idx=$(($part_args_idx+1))

		if ! [ "$select_num" -eq "$select_num" 2>/dev/null ]; then
			continue
		elif [ "$select_num" -le "${#sel_part_tbl[@]}" ] && [ "$select_num" -ge "1" ]; then
			if [ -z "${sel_part_arr[$select_num]}" ]; then
				sel_part_arr[$select_num]='*'
			else
				sel_part_arr[$select_num]=
			fi

			if [ "$disable_auto_select_app" -eq "0" ]; then
				if [ "${sel_part_tbl[$(($select_num-1))]}" = "app.bin" ]; then
					sel_part_arr[$part_appb_bin_idx]=${sel_part_arr[$select_num]}
				elif [ "${sel_part_tbl[$(($select_num-1))]}" = "app_b.bin" ]; then
					sel_part_arr[$part_app_bin_idx]=${sel_part_arr[$select_num]}
				fi
			fi
		fi

		if [ "${#part_args_arr[@]}" -ne "0" ] && [ "$part_args_idx" -ge "${#part_args_arr[@]}" ]; then
			break;
		fi
	done

	set -- $partition_tbl
	final_part_tbl=
	i=1
	while [ ! -z $1 ]; do
		if [ "${sel_part_tbl[$(($i-1))]}" == "$(basename $1)" ]; then
			if [ ! -z "${sel_part_arr[$i]}" ]; then
				if [ "$(basename $1)" = "app.bin" ] || [ "$(basename $1)" = "app_b.bin" ] ; then
					final_part_tbl="$final_part_tbl $1 $((($(stat -c %s $1)+$flash_sector_size-1)&(~($flash_sector_size-1))))"
				else
					final_part_tbl="$final_part_tbl $1 $2"
				fi
			fi
			i=$(($i+1))
		fi

		shift 2
	done

	partition_tbl=$final_part_tbl
}

if [ ! -z "$get_offset_bybin" ]; then
	$GEN_PARTMAP -s $start_addr -b $flash_sector_size -g $get_offset_bybin $partition_tbl
else
	if [ ! -d $PACK_DIR ]; then
		mkdir -p $PACK_DIR
	fi

	#echo "\n\n\n\n\tGenerator $output_file"

	if [ "$custom_mode" -eq "1" ]; then				# update app
		set -- $partition_tbl
		custom_tbl=
		while [ ! -z "$1" ]; do
			if [ "$(basename $1)" = "app.bin" ] || [ "$(basename $1)" = "app_b.bin" ] ; then
				if [ -f $1 ]; then
					custom_tbl="$custom_tbl $1 $((($(stat -c %s $1)+$flash_sector_size-1)&(~($flash_sector_size-1))))"
				fi
			fi
			shift 2
		done
		partition_tbl=$custom_tbl
	elif [ "$custom_mode" -eq "2" ]; then			# update all
		if [ -f "$BIN_DIR/all.bin" ]; then
			partition_tbl="$BIN_DIR/all.bin $((($(stat -c %s $BIN_DIR/all.bin)+$flash_sector_size-1)&(~($flash_sector_size-1))))"
		else
			partition_tbl=
		fi
	elif [ "$custom_mode" -eq "3" ]; then			# update custom all
		custom_allpart
	fi

	if [ -z "$partition_tbl" ]; then
		echo "Partition table is NULL!"
		exit 1
	fi

	if [ "$show_log" -eq "1" ]; then
		echo "    vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv"
	fi

	set -- $partition_tbl

	pack_list=
	pack_map_list=
	while [ ! -z "$1" ]; do
		if [ "$custom_mode" -eq "0" ] && [ "$(basename $1)" = "app_b.bin" ]; then
			file_size=0
		elif [ -f $1 ];  then
			file_size=$(stat -c %s $1)
		else
			file_size=0
		fi
		file_max_size=$(($2))

		if [ "$file_size" -gt "$file_max_size" ]; then
			printf "\n\n%s file size: \E[0;1;31;40m%d > %d\E[0m\n" $1 $file_size $file_max_size
			printf "\E[0;1;31;40mError\E[0m: Please Check \E[0;1;31;40m$(basename $1)\E[0m size setting in \E[0;1;31;40m$include_file\E[0m\n\n\n"
			exit 1
		fi

		if [ "$show_log" -eq "1" ]; then
			printf "\t%-30s %10s / %10s (%3d.%02d%%) used\n" $(basename $1) "$(printf "0x%x" $file_size)" "$(printf "0x%x" $file_max_size)" $((($file_size * 100) / $file_max_size)) $((((($file_size * 100) % $file_max_size) * 100) / $file_max_size))
		fi

		out_fill_fn=$PACK_DIR/$(basename $1).fill
		out_part_fn=$PACK_DIR/$(basename $1).part
		rm -f $out_fill_fn
		rm -f $out_part_fn

		cat /dev/null >> $out_fill_fn
		cat /dev/null >> $out_part_fn

		if [ "$custom_mode" -eq "0" ] && [ "$(basename $1)" = "app_b.bin" ] && [ -z "$3" ]; then
			pack_map_list="$pack_map_list $1 $out_part_fn $2"
			pack_list="$pack_list $out_part_fn"
			shift 2
			continue
		fi

		if [ -z "$3" ]; then
			if [ "$(basename $1)" = "app.bin" ] || [ "$(basename $1)" = "app_b.bin" ] ; then
				dd if=/dev/zero of=$out_fill_fn bs=$((($file_size+$flash_sector_size-1)&(~($flash_sector_size-1)))) count=1 status=none
				dd if=$1 of=$out_fill_fn conv=notrunc bs=$((($file_size+$flash_sector_size-1)&(~($flash_sector_size-1)))) count=1 status=none 2>/dev/null
			else
				dd if=/dev/zero of=$out_fill_fn bs=$file_max_size count=1 status=none
				dd if=$1 of=$out_fill_fn conv=notrunc bs=$file_max_size count=1 status=none 2>/dev/null
			fi
		else
			if [ "$(basename $1)" = "app.bin" ] && [ "$(basename $3)" = "app_b.bin" ] && [ -z "$5" ]; then
				dd if=/dev/zero of=$out_fill_fn bs=$((($file_size+$flash_sector_size-1)&(~($flash_sector_size-1)))) count=1 status=none
				dd if=$1 of=$out_fill_fn conv=notrunc bs=$((($file_size+$flash_sector_size-1)&(~($flash_sector_size-1)))) count=1 status=none 2>/dev/null
			else
				dd if=/dev/zero of=$out_fill_fn bs=$file_max_size count=1 status=none
				if [ "$custom_mode" -ne "0" ] || [ "$(basename $1)" != "app_b.bin" ]; then
					dd if=$1 of=$out_fill_fn conv=notrunc bs=$file_max_size count=1 status=none 2>/dev/null
				fi
			fi
		fi

		if { [ "$(basename $1)" = "app.bin" ] || [ "$(basename $1)" = "app_b.bin" ]; } && [ "$custom_mode" -gt "0" ]; then
			$MODIFYUPGRADEFLAG $out_fill_fn 0 > /dev/null
		fi

		if [ "$xb_en" -eq "0" ]; then
			cp -f $out_fill_fn $out_part_fn
		else
			if [ "$(basename $1)" = "app.bin" ] || [ "$(basename $1)" = "app_b.bin" ] ; then
				$XB2 -k 128 0x70 $out_fill_fn $out_part_fn
			else
				$XB2 -k 0 0x70 $out_fill_fn $out_part_fn
			fi
		fi

		pack_map_list="$pack_map_list $1 $out_part_fn $2"
		pack_list="$pack_list $out_part_fn"
		shift 2
	done

	if [ "$show_log" -eq "1" ]; then
		echo "    vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv"
	fi

	if [ ! -z $ispbin_file ]; then
		$GEN_PARTMAP -s $start_addr -b $flash_sector_size -o $PACK_MAP_BIN $pack_map_list
	else
		$GEN_PARTMAP -u -s $start_addr -b $flash_sector_size -o $PACK_MAP_BIN $pack_map_list
	fi

	if [ "$xb_en" -eq "1" ]; then
		$XB2 -k 0 0x70 $PACK_MAP_BIN $PACK_DIR/$(basename $PACK_MAP_BIN)
		PACK_MAP_BIN=$PACK_DIR/$(basename $PACK_MAP_BIN)
		if [ ! -z $ispbin_file ]; then
			$XB2 -w 1 0x70 $ispbin_file $PACK_DIR/$(basename $ispbin_file).part
			ispbin_file=$PACK_DIR/$(basename $ispbin_file).part
		fi
	fi

	rm -f $output_file
	cat $ispbin_file $PACK_MAP_BIN $pack_list > $output_file
	if [ "$custom_mode" -eq "3" ]; then			# update custom all
		echo "Generator $(basename $output_file) OK!"
	fi
fi


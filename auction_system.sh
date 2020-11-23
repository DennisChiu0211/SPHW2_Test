#!/bin/bash 
n_host=$1
n_player=$2
key_list=()
final_score=()
for((i=0; i<n_player; i++));
do
	final_score+=(0)
done
for((i=0; i<=$n_host; i++));
do
	mkfifo fifo_$i.tmp
	key_list+=($RANDOM)
	if [ $i -lt $n_host ];then
		./host $[i+1] ${key_list[$i]} 0 &
	fi
done

for((i=3; i <= $n_host + 3; i++));
do
	eval "exec $i<> fifo_$[i-3].tmp"
done

game=1
for((i=9; i <= $n_player; i++));
do
	((game = game*i/($i-8)))
done

now=4
for ((a=1; a <= $n_player; a++));
do
	for ((b=$a+1; b <= $n_player; b++));
	do
		for ((c=$b+1; c <= $n_player; c++));
		do 
			for ((d=$c+1; d <= $n_player; d++));
			do 
				for ((e=$d+1; e <= $n_player; e++));
				do 
					for ((f=$e+1; f <= $n_player; f++));
					do
						for ((g=$f+1; g <= $n_player; g++));
						do 
							for ((h=$g+1; h <= $n_player; h++));
							do
								if [ $n_host -le $game ];then
									if [ $now -le $[n_host+3] ];then
										eval "echo $a $b $c $d $e $f $g $h >&$now"
										((now++))
										continue
									else
										read key<&3
										for ((i=1; i<=$n_host; i++));
										do
											if [ $key -eq ${key_list[$[i-1]]} ];then
											
												eval "echo $a $b $c $d $e $f $g $h >&$[$i+3]"
											fi
										done
										for ((i=0; i < 8; i++));
										do
											read id rank<&3
											((final_score[$id-1]+=(8-$rank)))
										done
									fi
								else
									eval "echo $a $b $c $d $e $f $g $h >&$now"
								fi
							done
						done
					done
				done
			done
		done
	done
done
if [ $n_host -le $game ];then
	for ((i=0; i < $n_host; i++));
	do 
		read key <&3
		for ((j=0; j < 8; j++));
		do
			read id rank<&3
			((final_score[$id-1]+=(8-$rank)))
		done
	done
else
	
	for ((i=0; i < $game; i++));
	do
		read key<&3
		for ((j=0; j < 8; j++));
		do
			read id rank<&3
			((final_score[$id-1]+=(8-$rank)))
		done
	done
fi

for ((i=1; i<=$n_host; i++));
do
	echo "-1 -1 -1 -1 -1 -1 -1 -1" > fifo_$i.tmp
done

for ((i=0; i<$n_player; i++));
do
	echo $[i+1] ${final_score[$i]}
done

rm -f fifo*



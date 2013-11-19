
2.	a)

	14	13	12	11	10	9	8	7	6	5	4	3	2	1	0
	CT	CT	CT	CT	CT	CT	CT	CT	CT	CI	CI	CI	CO	CO	CO
	
	b)
	
	Operation		Address			Cache Set		Hit?			Value
	Read			0x0117			2			MISS			N/A
	Read			0x64C2			0			HIT			99
	Read			0x7BFE			7			MISS			N/A
	Read			0x0110			2			HIT			Unknown
	
3.	a) 
	sumA accesses the array in row major order. Therefore, upon the fist 
	load, it would load 16 bytes of memory into a line into one of the 
	cache sets. Therefore, the next 3 accesses will be cache hits at which
	point, the first access ( 4 bytes ) plus the subsequent 3 accesses 
	(3 * 4 bytes = 	12 bytes) would equal 16 bytes, thus exceeding the size
	of one cache line and the next access, or the 5th access will be a cache 
	miss at which point the description above would repeat. Therefore, for 
	every 1 cache miss, there are 3 cache hits. Thus out of the 64 * 64 = 4096
	accesses, 1024 will be misses and so it will be a 25% miss rate overall. 

	b)
	sumB accesses the array in column major order and so on the first array 
	access it would load 16 bytes of memory into the cache, into some cache set.
	If we arbitrarily choose cache set 0 as the set it was loaded into then on the
	subsequent memory access it would load a value 16 * 4 = 256 bytes ahead in
	memory which would cause 16 bytes of memory to be loaded into cache set 16.
	This would continue for 8 accesses. At which point, the 9th access would 
	overwrite the value written to by the first access. And so there would never 
	be a cache hit. Resulting in a miss rate of 100%
	
	c)
	sumC accesses the values in the array by reading the value in the current 
	index then the adjacent row, adjacent column, and the adjacent row and column. 
	For the first four accesses, the first access to each row is a miss and the 
	next accesses to the new element in the row is a hit. The general progression 
	however is column major which has the same problem as b). Thus every four 
	accesses has two misses and two hits results in a miss rate of one half.

	d)
	68 rows and columns introduces a new problem since after the end of the first
	column is reached, all of the values in the first column are stored in different
	sets. This is because 68 is not divisible by 16, which is the size of each block.
	Thus when the second, third and fourth columns are iterated over, the values have
	already been loaded ino the cache. This repeats, causing the miss rate to be 1/4.
	
	
	
	
Log  1: convert_try1:
	Assign the values to the G array coumn first to match the way they are layed out in memory. The OR statement is also inverted, the original was: 
												G[j][i] = G[j][i] || G[i][j];
												and the new one is:
												G[i][j] = G[j][i] || G[i][j] ;
	since the first value read from memory is G[j][i], and the second value is G[i][j], if the first value happens to be allocated to the same area in the cache as the second value, the second value would overwrite the first.
	Therefore, we would want the value being written to be the same address as the value being read from second. Thus, it makes sense to flip the OR.

Log  2: convert_try2:
	Iterate over the G array column first. But since we know at some point (i, j) what the value of (j, i) should be, just change it there instead of waiting for the iteration to reach (j, i) since its already cached when the value of it is read to assign the value of (i, j). Essentially assign the value of (j, i) when we reach (i, j). Skip the iteration for (j, i)


TABLE:

Performance results:

Entry				Average Time			Improvement Ratio

original		1179048						1
log 1				769601						0.65
log 2				598540						0.50

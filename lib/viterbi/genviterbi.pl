# Perl script to generate C code for an encoder and 
# a Viterbi decoder for any specified rate 1/N, constraint length k <= 16 
# convolutional code. 
# 
# Not recommended for large K codes, as it will generate a *very* large 
# program! 
 
# 24 October 1997 Phil Karn, KA9Q 
 
# Here are the polynomials for some optimum convolutional codes. Select only one 
# by uncommenting it 
 
# Rate 1/2 codes 
#@polys = (0x7,0x5);		# k=3 
#@polys = (0xf,0xb);		# k=4 
#@polys = (0x17, 0x19);		# k=5 
#@polys = (0x13, 0x1b);		# k=5, used in GSM? 
#@polys = (0x2f, 0x35);		# k=6 
@polys = (0x6d,0x4f);		# k=7; very popular with NASA and industry 
#@polys = (0x9f, 0xe5);		# k=8 
#@polys = (0x1af, 0x11d);	# k=9; used in IS-95 CDMA 
#@polys = (0x45dd, 0x69e3);	# k = 15 
 
 
# Rate 1/3 codes 
#@polys = (0x7, 0x7, 0x5);	#k = 3 
#@polys = (0xf, 0xb, 0xd);	#k = 4 
#@polys = (0x1f, 0x1b, 0x15);	#k = 5 
#@polys = (0x2f, 0x35, 0x39);	#k = 6 
#@polys = (0x4f, 0x57, 0x6d);	#k = 7; also popular with NASA and industry 
#@polys = (0xef, 0x9b, 0xa9);	#k = 8 
#@polys = (0x1ed, 0x19b, 0x127); #k = 9; used in IS-95 CDMA 
 
# end of user-configurable stuff 
 
# Figure out constraint length by looking at the polynomials 
$bar = 0; 
foreach $i (@polys){ 
	$bar |= $i; 
} 
$k = 0; 
while($bar != 0){ 
	$k++; 
	$bar >>= 1; 
} 
die "Max K=16 supported" if ($k > 16); 
 
$nsym = @polys; 
$nmets = 1 << $nsym; 
$smask = $nmets - 1; 
$K = 1 << ($k - 1); 
if($K/32 >= 1){ 
	$D = $K/32; 
} else { 
	$D = 1; 
} 
 
 
print "unsigned char Partab[] = { "; 
for $i (0 ... 255) { 
	print parity($i),","; 
} 
print "}; 
"; 
 
print "/* Convolutional encoder */ 
encode( 
unsigned char *symbols, 
unsigned char *data, 
unsigned int nbytes) 
{ 
	unsigned int encstate; 
	int i; 
 
	encstate = 0; 
	while(nbytes-- != 0){ 
		for(i=7;i>=0;i--){ 
			encstate = (encstate << 1) | ((*data >> i) & 1);\n"; 
 
foreach $poly (@polys) { 
	if($k <= 8){ 
		print "\t\t\t*symbols++ = Partab[encstate & $poly];\n"; 
	} elsif($k <= 16){ 
		print "\t\t\t*symbols++ = Partab[((encstate & $poly) ^ ((encstate & $poly) >> 8)) & 0xff];\n"; 
	} 
} 
	 
print "\t\t} 
		data++; 
	} 
	return 0; 
}\n"; 
 
print "/* Viterbi decoder */ 
int 
viterbi( 
unsigned long *metric,	/* Final path metric (returned value) */ 
unsigned char *data,	/* Decoded output data */ 
unsigned char *symbols,	/* Raw deinterleaved input symbols */ 
unsigned int nbits,	/* Number of output bits */ 
int mettab[2][256]	/* Metric table, [sent sym][rx symbol] */ 
){ 
	unsigned int bitcnt = 0; 
	int beststate,i; 
	long cmetric[$K],nmetric[$K]; 
	register int m0,m1; 
 
	/* This next line is arguably illegal C, but it works on 
	 * GCC and it avoids having to reference this heavily used 
	 * array through a pointer. If you can't compile this, use malloc. 
	 */ 
	unsigned long paths[nbits*",$D,"*sizeof(unsigned long)]; 
	unsigned long *pp; 
	unsigned long dec; 
	int mets[$nmets]; 
	 
	pp = paths; 
	/* Initialize starting metrics to prefer 0 state */ 
	cmetric[0] = 0; 
	for(i=1;i<$K;i++) 
		cmetric[i] = -999999; 
 
	for(;;){\n"; 
 
emitbits(0); 
# Too much loop unrolling, and we blow out of the Pentium instruction cache 
if($k <= 7) { 
	emitbits(1); 
} else { 
	print "\t\tmemcpy(cmetric,nmetric,",$K*4,");\n"; 
} 
print"\t} 
	pp -= ",$D,"; 
	/* Chain back from terminal state to produce decoded data */ 
	memset(data,0,nbits/8); 
	for(i=nbits-$k;i >= 0;i--){ 
		if(pp[beststate >> 5] & (1 << (beststate & 31))){ 
			beststate |= $K;	/* 2^(K-1) */ 
			data[i>>3] |= 0x80 >> (i&7); 
		} 
		beststate >>= 1; 
		pp -= ",$D,"; 
	} 
	return 0; 
}\n"; 
 
exit; 
 
 
sub emit_butterfly(@) { 
 
	my $source,$dest,$sym,$odd,$i,$j; 
 
	$i = shift; 
	$j = $i * 2; 
	$odd = shift; 
	$sym = shift; 
	if($odd){ 
		$source = "nmetric"; 
		$dest = "cmetric"; 
	} else { 
		$source = "cmetric"; 
		$dest = "nmetric"; 
	} 
 
print "\t\t/* state $i, symbols ", bsplit($sym,$nsym)," */ 
		m0 = $source\[$i] + mets[$sym]; 
		m1 = $source\[",($i+$K/2),"] + mets[",$smask ^ $sym,"]; 
		$dest\[",2*$i,"] = m0; 
		if(m1 > m0){ 
			$dest\[",2*$i,"] = m1; 
			dec |= "; 
printf "0x%lx",1 << ((2*$i) & 31); 
print					"; 
		} 
		m0 -= (mets[$sym] - mets[",$smask^$sym,"]); 
		m1 += (mets[$sym] - mets[",$smask^$sym,"]); 
		$dest\[",2*$i+1,"] = m0; 
		if(m1 > m0){ 
			$dest\[",2*$i+1,"] = m1; 
			dec |= "; 
printf "0x%lx",1 << ((2*$i+1) & 31); 
print					"; 
		} 
" 
 
} 
sub emitbits(@) { 
 
	my $sym; 
 
	$odd = shift; 
 
	if($odd){ 
		$dest = "cmetric"; 
	} else { 
		$dest = "nmetric"; 
	} 
	foreach $i (0 ... $nmets-1){ 
		print "\t\tmets[$i] = "; 
		@symbits = bsplit($i,$nsym); 
		print "mettab[",pop @symbits,"][symbols[0]]"; 
		foreach $j (1 ... $nsym-1){ 
			print " + mettab[",pop @symbits,"][symbols[$j]]"; 
		} 
		print ";\n"; 
	} 
	print "\t\tsymbols += $nsym;\n"; 
	foreach $i (0 ... $K/2-1){ 
		$sym = 0; 
		foreach $p (@polys){ 
			$sym = $sym * 2 + parity((2*$i & $p)); 
		} 
		push @stuff,join ' ',$sym,$i; 
		if(($i % 16) == 15){ 
			print "\t\tdec = 0;\n"; 
			foreach $sym (sort @stuff){ 
				@a = split(' ',$sym); 
				emit_butterfly($a[1],$odd, $a[0]); 
			} 
			@stuff = (); 
			print "\t\t*pp++ = dec;\n"; 
		} 
	} 
	if($K/2 < 16){ 
			print "\t\tdec = 0;\n"; 
			foreach $sym (sort @stuff){ 
				@a = split(' ',$sym); 
				emit_butterfly($a[0],$odd, $a[1]); 
			} 
			@stuff = (); 
			print "\t\t*pp++ = dec;\n"; 
	} 
	print "\t\tif(++bitcnt == nbits){ 
			beststate = 0; 
			*metric = $dest\[beststate]; 
			break; 
		}\n"; 
} 
sub bsplit() 
{ 
	$x = shift; 
	$n = shift; 
	my @ret; 
 
	while($n-- > 0){ 
		push @ret,($x & 1); 
		$x = $x >> 1; 
	} 
	return @ret; 
} 
sub parity() 
{ 
	$x = shift; 
 
	$x ^= ($x >> 16); 
	$x ^= ($x >> 8); 
	$x ^= ($x >> 4); 
	$x ^= ($x >> 2); 
	$x ^= ($x >> 1); 
	return ($x & 1); 
} 
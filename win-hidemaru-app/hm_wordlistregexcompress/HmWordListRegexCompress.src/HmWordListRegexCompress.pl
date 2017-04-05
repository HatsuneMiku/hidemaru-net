use Regex::PreSuf;

my $HIDE_LINE_CHAR_LIMIT = 240;

if (@ARGV >= 1 && -e $ARGV[0]) {
	
	if (@ARGV >= 2 && $ARGV[1] =~ /^\d+$/) {
		$HIDE_LINE_CHAR_LIMIT = $ARGV[1];
	}
	
	# �t�@�C������ǂݍ��ݔz��ցB
	open FH, $ARGV[0] or die "�����̃t�@�C�����s���Ȃ悤�ł��B";
	my @lines = <FH>;
	close FH;
	
	# 1�s���ǂݍ���ŁA����܂ł̒P��������āA���k�������ʂ� $HIDE_LINE_CHAR_LIMIT�����Ȃ�A��������ł���
	my $iCurBufLength = 0;
	my @listCurBuf = ();
	for my $line (@lines) {
		chomp($line);
		# �����Ŏ��̒P��𑫂��Ă݂�
		my @nextCurBuf = @listCurBuf;
		push @nextCurBuf, $line;
		
		# ���K�\�����k���Ă݂�
		my $reNext = presuf(@nextCurBuf);
				
		# �͂ݏo������A�o��
		if (length($reNext) > $HIDE_LINE_CHAR_LIMIT) {
			PrintRegex(@listCurBuf);
			
			# ������
			$iCurBufLength = 0;
			@listCurBuf = ();

		}
		
		# �����$line��Buf�ւƉ�����			
		$iCurBufLength = $iCurBufLength + length($line);
		push @listCurBuf, $line;
		
	}
	
	# �Ō�c�肪����Ώo��
	if ($iCurBufLength > 0) {
		PrintRegex(@listCurBuf);
	}	
}

sub PrintRegex {
	my $reCurr = presuf(@_);
	$reCurr =~ s/\(\?\:/\(/g;
	print($reCurr, "\n");
}
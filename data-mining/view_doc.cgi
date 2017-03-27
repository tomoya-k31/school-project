#!/usr/bin/perl

use strict;
use Encode;
use utf8;

use File::Basename;
use File::Find;

use KyotoCabinet;
my $tfidf_db = new KyotoCabinet::DB;
my $tfidf_file = "db/tfidf.kch";
my $index_db = new KyotoCabinet::DB;
my $index_file = "db/index.kch";
my $index_ad_db = new KyotoCabinet::DB;
my $index_ad_file = "db/index_advance.kch";
my $title_db = new KyotoCabinet::DB;
my $title_file = "db/title.kch";

my $N = 188627;

use CGI;
my $q = new CGI;

main();

sub main
{
  my $forms = decode_utf8($ARGV[0]);
  my @form = split(/\\&/,$forms);
  my $main_file = $form[0];
  my $words = $form[1];
  my @word = split(/:/,$words);
  print_html_header();

# タイトルを取得
  my $title = extractTitle($main_file);

  print '<div class="index_study">';
  print encode_utf8("<h3>$title</h3><p>\n");

# 本文を取得
  my @sentence = getSentence($main_file);

  foreach my $sent (@sentence)
  {
    foreach (@word){
      $sent =~ s/$_/<span style="color:#ff0000;">$_<\/span>/g;
      # $sent =~ s/$_/<span style="
      # border-bottom:solid 1px #ff0000;">$_<\/span>/g;
    }
    print encode_utf8("$sent\n");
  }

#-------------------------------------------------------------------------------

  print "<p></div>\n<div><h3>関連記事</h3><ul>\n";

  my %tfidf_hash;
  my %count_hash;

  $index_ad_db->open($index_ad_file, $index_ad_db->OREADER);

  foreach (@word){
    my $result_str = $index_ad_db->get($_);

    my @result_list = split(/\,/,$result_str);

    my $df = @result_list;
    foreach (@result_list){
      my @tmp = split(/:/,$_);
      my $tf = $tmp[1];
      my $tfidf = $tf*log($N/$df)/log(2);
      $tfidf_hash{$tmp[0]} += $tfidf;
      $count_hash{$tmp[0]}++;
    }
    undef @result_list;
  }

  my @list = sort_hash_value(\%tfidf_hash);
  my $len = @word;
  my %total_vector;
  my $c;
  foreach my $file (@list)
  {
    if($c<3 && $count_hash{$file}==$len){
      $c++;
      my %tmp_vec = create_vector($file);
      %total_vector = sum_hash(\%total_vector,\%tmp_vec);
    }
  }

  my @search_result = search_article(\%total_vector);

  my %score_hash;

  foreach my $file (sort @search_result)
  {
    my %result_vector = create_vector($file);
    my $cos = calc_cosin(\%total_vector,\%result_vector);
    if($cos > 0.3)
    {
      $score_hash{$file} = $cos;
    }
  }
  undef @search_result;

  my @sort_result = sort_hash_value(\%score_hash);
  my $count = 0;

  foreach my $file (@sort_result)
  {
    $file eq $main_file?next:$count++;
    $count==4?last:0;
    my $title = extractTitle($file);
    print encode_utf8("<li>・<a href=\"view_doc.cgi?$file\&$words\">$title</a></li>\n");
  }
  print "</ul></div>\n";

  print_html_fotter();
  $index_ad_db->close();
}


sub getSentence
{
  my($file) = @_;

# ファイルをオープン
  open(IN,$file) || die "Can't open $file\n";

  my @Sentence;

  while(my $line_utf8 = <IN>)
  {
    my $line = decode_utf8($line_utf8); 
    chomp($line);

    my @element = split(/ /,$line);
    my $tag = $element[0];

# 先頭にタグがある行を除去
    if($tag eq "<id>" || $tag eq "<date>" || $tag eq "<title>")
    {
      next;
    }

    $line =~ s/^　//;  # 先頭の全角スペースを除去
      $line =~ s/^ //;   # 先頭の半角スペースを除去

# 何もない行を除去
      if($line eq "")
      {
        next;
      }

    undef @element;

    push(@Sentence,$line);
  }
  close(IN);

  return @Sentence;
}

sub extractTitle
{
  my($file) = @_;

  open(SGML,$file) || die "Can't open $file\n";

  my $data;
  while(my $line_utf8 = <SGML>)
  {
    my $line = decode_utf8($line_utf8);
    chomp($line);

# splitで取得
    my($tag,$data) = split(/ /,$line);
    if($tag eq "<title>")
    {
      return $data;
    }
  }
  close(SGML);
}


sub create_vector
{
  $tfidf_db->open($tfidf_file, $tfidf_db->OREADER);

  my $tfidf = decode_utf8($tfidf_db->get(basename($_[0])));
  my @tmp = split(/,/,$tfidf);
  my %tfidf_hash;

  foreach my $pair (@tmp)
  {
    my @kv = split(/:/,$pair);
    if($kv[1] >= 20)
    {
      $tfidf_hash{$kv[0]} = $kv[1];
    }
  }
  $tfidf_db->close();
  return %tfidf_hash;
}


sub search_article
{
  my $ref_hash = $_[0];
  my %object_vector = %{$ref_hash};

  my @search_result;
  my %search_hash;

  $index_db->open($index_file, $index_db->OREADER);

    foreach (keys %object_vector)
    {
      my $tmp = decode_utf8($index_db->get($_));
      my @list = split(/,/,$tmp);
      foreach (@list){
        $search_hash{$_} = 1;
      }
    }
    foreach (keys %search_hash){
      push(@search_result,$_);
    }
  $index_db->close();
  return @search_result;
}

sub calc_cosin
{
  my ($ref_object_hash,$ref_result_hash) = @_;
  my %object_hash = %{$ref_object_hash};
  my %result_hash = %{$ref_result_hash};

  my $sop = 0;

  my $V1 = calc_vector(%object_hash);
  my $V2 = calc_vector(%result_hash);

  foreach my $key (keys %object_hash)
  {
    if(exists($result_hash{$key}))
    {
      $sop += $object_hash{$key}*$result_hash{$key};
    }
  }
  return ($V1*$V2)?$sop/($V1*$V2):0;
}

sub calc_vector
{
  my %tfidf_hash = @_;
  my $sum = 0;

  foreach my $key (keys %tfidf_hash)
  {
    $sum += $tfidf_hash{$key} * $tfidf_hash{$key};
  }

  return sqrt($sum);
}

sub sort_hash_value
{
  my($ref_hash) = @_;
  my(@keys);
  @keys = sort { $$ref_hash{$b} <=> $$ref_hash{$a} } keys %{$ref_hash};
  return @keys;
}



sub sum_hash
{
  my($ref_total,$ref_new) = @_;
  my %total_vec = %{$ref_total};
  my %vec = %{$ref_new};

  foreach (keys %vec)
  {
    if(exists($total_vec{$_}))
    {
      $total_vec{$_} += $vec{$_};
    }
    else
    {
      $total_vec{$_} = $vec{$_};
    }
  }
  return %total_vec;
}



sub print_html_header
{
    print << "HERE";

<!DOCTYPE html>
<html>

<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<link rel="stylesheet" href="css/lalab.css" type="text/css">
<title>EasySearch</title>
</head>

<body>

<!-- Header -->
<div id="header">
<div id="lab-name">Information Retrieval</div>
</div>

<!-- Input field -->
<div id="contents" class="theme-blue"> 
<div id="index_studies">

HERE
}

sub print_html_fotter
{
    print << "HERE";

</div>
</div>

<!-- fotter -->
<div id="fotter">
  <div><a href="http://www.ci.seikei.ac.jp/sakai/">Language Information Laborato
ry</a></div>
</div>

</body>
</html>

HERE

}

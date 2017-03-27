#!/usr/bin/perl 

use strict;
use Encode;
use utf8;
use File::Find;
use File::Basename;

use KyotoCabinet;
my $index_ad_db = new KyotoCabinet::DB;
my $index_ad_file = "db/index_advance.kch";
my $title_db = new KyotoCabinet::DB;
my $title_file = "db/title.kch";

my $N = 188627;

use CGI;
my $q = new CGI;

main();

sub main()
{
  print_html_header();

  $title_db->open($title_file, $title_db->OREADER);

# 検索キーワードを取得
  my $query = decode_utf8($q->param('keyword'));
  $query =~ s/　/ /g;
  my $send = $query;
  $send =~ s/ /:/g;

# 検索エンジン（too_easy.pl）
  my @result = easy_search($query);

# 検索結果を表示
  print "<ul>\n";
  foreach my $file (@result)
  {
    my $url = "view_doc.cgi?$file\&$send";
# my $b_name = basename($file);
    my $title = decode_utf8($title_db->get(basename($file)));

    print encode_utf8("<li style=\"font-size: 16px;\">
        ・<a href=$url>$title</a></li>\n");
  }
  print "</ul>\n";

  print_html_fotter();
  
  $title_db->close();

}

sub easy_search
{
  my($query) = @_;
  my @query2 = split(/"/,$query);
  my @keys = split(/ /,$query2[0]);

  my @result;

  my %tfidf_hash;
  my %count_hash;

  $index_ad_db->open($index_ad_file, $index_ad_db->OREADER);


  foreach (@keys){
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
  my $len = @keys;
  foreach my $file (@list)
  {
    if($count_hash{$file}==$len){
      push(@result,$file);
    }
  }

  $index_ad_db->close();

  return @result;
}

sub sort_hash_value
{
  my($ref_hash) = @_;
  my(@keys); 
  @keys = sort { $$ref_hash{$b} <=> $$ref_hash{$a} } keys %{$ref_hash};
  return @keys;
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
    <div id="contents" class="theme-blue padding-none">
    <div id="index_intro">

    <table width="75%">
    <tr><td>　</td></tr>
    <tr><td align="center">
    <form method=post action="easy_search.cgi">
    <input type=text name="keyword" size=50>
    <input type=submit value="検索">
    <input type=reset value="クリア">
    </form>
    </td></tr>
    </table>

    </div>
    </div>

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

#!/usr/bin/perl
use CGI;

my $cgi = new CGI;

print $cgi->header,
      $cgi->start_html('CGI Version Info'),
      "CGI.pm Version: $CGI::VERSION",
      $cgi->end_html;


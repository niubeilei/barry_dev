#!/usr/bin/perl -w

use strict;
require aos_parsers;

my $parsers = aos_parsers::get_parsers();
my $global_states = aos_parsers::get_global_states();

open(SOURCE, ">aos_parser.c") 
	or die "Failed to open aos_parser.c: $!\n";
open(HEADER, ">aos_parser.h")
	or die "Failed to open aos_parser.h: $!\n";

my $now = localtime(time);

print HEADER "/*\n";
print HEADER " * This file was automatically generated\n";
print HEADER " * $now by aos_gen_parsers.pl\n";
print HEADER " */\n\n";

print HEADER "#ifndef __AOS_PARSER_H__\n";
print HEADER "#define __AOS_PARSER_H__\n\n";
print HEADER "#include <stdint.h>\n\n";
print HEADER "#define MAX_STATE_ROWS	256\n";

print SOURCE "/*\n";
print SOURCE " * This file was automatically generated\n";
print SOURCE " * $now by aos_gen_parsers.pl\n";
print SOURCE " */\n\n";

#print SOURCE "#include <sys/types.h>\n";
print SOURCE "#include <stdlib.h>\n";
print SOURCE "#include <stdio.h>\n";
print SOURCE "#include \"aos_parser.h\"\n\n";

my $parser;
my $state;
my %state_names;
my $global_max = 0;

foreach $parser (@$parsers) {

	undef %state_names;
	my %state_names;

	my $table_name = "$parser->{name}_table";
	my $parser_ucase = "\U$parser->{name}";

	my $max = 0;
	foreach $state (@{$parser->{states}}) {
		if ($state->{id} > $max) {
			$max = $state->{id};
		}
	}

	my $state_count = $max + 1;
	my $special_state = $state_count;
	
	print HEADER "\n/* ---- $parser->{name} defines and functions ---- */\n";
	print HEADER "#define ${parser_ucase}_MAX $max\n";
	print HEADER "#define ${parser_ucase}_INITIAL $parser->{initial}\n";

	print SOURCE "\nstatic void ${table_name}_init(void);\n";
	print SOURCE "uint16_t ${table_name}" . 
				 "[MAX_STATE_ROWS][$state_count];\n";
	print SOURCE "\nstatic void ${table_name}_init(void)\n{\n";
	print SOURCE "\tint32_t i;\n";

	if (defined($parser->{global_default})) {
		if ($parser->{global_default} !~ /^\d+$/) {
			if (!&is_global($parser->{global_default})) {
				if (!exists($state_names{$parser->{global_default}})) {
					$state_names{$parser->{global_default}} = $special_state++;
				}
				$parser->{global_default} = "\U$parser->{name}".
			                       "\E_$parser->{global_default}";
			}
		} elsif (!&state_exists($parser, $parser->{global_default})) {
			  	die "Error while generating global default of parser " .
			  	     "$parser->{name}: state $parser->{global_default} does " .
			  	     "not exist\n";
		}
			
		print SOURCE "\n\tfor (i = 0; i < MAX_STATE_ROWS; i++) {\n";
		print SOURCE "\t\tint32_t j;\n";
		print SOURCE "\t\tfor (j = 0; j < $state_count; j++) {\n";
		print SOURCE "\t\t\t$table_name" .
					 "[i][j] = $parser->{global_default};\n";
		print SOURCE "\t\t}\n";
		print SOURCE "\t}\n";
	}

	foreach $state (@{$parser->{states}}) {

		print SOURCE "\n\t/* ----[ State $state->{id} ]---- */\n";

		if (defined($state->{canonical_name})) {
			print HEADER "#define \U$parser->{name}\E_$state->{canonical_name}".
			             " $state->{id}\n";
		}

		if (defined($state->{default})) {
			if ($state->{default} !~ /^\d+$/) {
			    if (!&is_global($state->{default})) {
					if (!exists($state_names{$state->{default}}) &&
						!&state_name_exists($parser, $state->{default})) {
						$state_names{$state->{default}} = $special_state++;
					}
					$state->{default} = "\U$parser->{name}\E_$state->{default}";
			    } 
			} elsif (!&state_exists($parser, $state->{default})) {
				  	die "Error while generating state $state->{id} of parser " .
				  		"$parser->{name}: state $state->{default} does not exist\n";
			}

			print SOURCE "\tfor (i = 0; i < MAX_STATE_ROWS; i++) {\n";
			print SOURCE "\t\t$table_name" . 
					 "[i][$state->{id}] = $state->{default};\n";
			print SOURCE "\t}\n\n";
		} elsif (!defined($parser->{global_default})) {
			die "Error while generating state $state->{id} of parser " .
				"$parser->{name}: state does not have a default transition, ".
				"and no global default was defined\n";
		}

		my $transition;

		foreach $transition (@{$state->{transitions}}) {

			if ($transition->{result} !~ /^\d+$/) {
			    if (!&is_global($transition->{result})) {
					if (!exists($state_names{$transition->{result}}) &&
			    		!&state_name_exists($parser, $transition->{result})) {
					    $state_names{$transition->{result}} = $special_state++;
					} 
					$transition->{result} = "\U$parser->{name}\E_$transition->{result}";
			    }
			} elsif (!&state_exists($parser, $transition->{result})) {
				die "Error while generating state $state->{id} of parser " .
					"$parser->{name}: state $transition->{result} does " .
					"not exist\n";
			}

			if ($transition->{input} eq "SINGLEQUOTE") {
				&single_quote($table_name, $state->{id}, $transition->{result});

			} elsif ($transition->{input} eq "WHITE_SPACE") {
				&white_space($table_name, $state->{id}, $transition->{result});

			} elsif ($transition->{input} eq "CRLF") {
				&crlf_space($table_name, $state->{id}, $transition->{result});

			} elsif ($transition->{input} eq "WS_NON_CRLF") {
				&noncrlf_space($table_name, $state->{id}, $transition->{result});

			} elsif ($transition->{input} eq "HTTP_CR") {
				&http_cr($table_name, $state->{id}, $transition->{result});

			} elsif ($transition->{input} eq "HTTP_LF") {
				&http_lf($table_name, $state->{id}, $transition->{result});

		        } elsif ($transition->{input} eq "HTTP_TAB_SPACE") {
				&http_tab_space($table_name, $state->{id}, $transition->{result});

			} elsif ($transition->{input} eq "HTTP_ALPHA") {
				&http_alpha($table_name, $state->{id}, $transition->{result});

			} elsif ($transition->{input} eq "HTTP_DIGIT") {
				&http_digit($table_name, $state->{id}, $transition->{result});

			} elsif ($transition->{input} eq "HTTP_PRNCHAR") {
				&http_prnchar($table_name, $state->{id}, $transition->{result});

			} elsif ($transition->{input} eq "HTTP_EXTCHAR") {
				&http_extchar($table_name, $state->{id}, $transition->{result});

			} elsif ($transition->{input} eq "HTTP_CTLCHAR") {
				&http_ctlchar($table_name, $state->{id}, $transition->{result});

			} elsif ($transition->{input} eq "HTTP_ESCCHAR") {
				&http_escchar($table_name, $state->{id}, $transition->{result});

			} elsif ($transition->{input} eq "NULL") {
				print SOURCE "\t$table_name" .
				             "[\'\\0\'][$state->{id}] = " .
				             "$transition->{result};\n";
			} else {
				my $input = $transition->{input};
				if ($input =~ /^[a-zA-Z]$/) {
					print SOURCE "\t$table_name" .
								 "[\'\U$input\'][$state->{id}] = " .
								 "$transition->{result};\n";
					print SOURCE "\t$table_name" .
								 "[\'\L$input\'][$state->{id}] = " .
								 "$transition->{result};\n";
				} else {
					print SOURCE "\t$table_name" .
								 "[\'$input\'][$state->{id}] = " .
								 "$transition->{result};\n";
				}
			}

		}
	}

	my $name_key;

	foreach $name_key (sort keys %state_names) {
		print HEADER "#define \U$parser->{name}\E_$name_key " .
					 "$state_names{$name_key}\n";
	}

	print SOURCE "}\n\n";

	print HEADER "\nextern uint16_t $table_name"."[MAX_STATE_ROWS][$state_count];\n";
	print HEADER "\nstatic __inline uint16_t $parser->{name}_next".
				"(uint16_t curr_state, uint16_t input)\n";
	print HEADER "{\n";
	print HEADER "\treturn ${table_name}"."[input][curr_state];\n";
	print HEADER "}\n\n";
	
	if ($special_state > $global_max) {
		$global_max = $special_state;
	}
}

my $global_state;

foreach $global_state (@$global_states) {
	print HEADER "#define \U$global_state\E $global_max\n";
	$global_max++;
}
print HEADER "\n";

print SOURCE "void aos_parser_init(void)\n{\n";
foreach $parser (@$parsers) {
	print SOURCE "\t$parser->{name}_table_init();\n";
}
print SOURCE "}\n";
print HEADER "\nvoid aos_parser_init(void);\n";

print HEADER "\n#endif /* __AOS_PARSER_H__ */\n";

close SOURCE;
close HEADER;

sub state_exists()
{
	my $parser = shift;
	my $search_state = shift;

	foreach $state (@{$parser->{states}}) {
		if ($search_state eq $state->{id}) {
			return 1;
		} 
	}

	return 0;
}

sub state_name_exists()
{
	my $parser = shift;
	my $search_state = shift;

	foreach $state (@{$parser->{states}}) {
		if (defined($state->{canonical_name})) {
			if ($search_state eq $state->{canonical_name}) {
				return 1;
			}
		}
	}

	return 0;
}

sub single_quote()
{
	my $table = shift;
	my $s = shift;
	my $r = shift;

	print SOURCE "\t${table}" . "['\\\''][$s] = $r;\n";
}

sub white_space()
{
	my $table = shift;
	my $s = shift;
	my $r = shift;

	print SOURCE "\t${table}" . "[' '][$s] = $r;\n";
	print SOURCE "\t${table}" . "['\\t'][$s] = $r;\n";
	print SOURCE "\t${table}" . "['\\r'][$s] = $r;\n";
	print SOURCE "\t${table}" . "['\\n'][$s] = $r;\n";
}

sub crlf_space()
{
	my $table = shift;
	my $s = shift;
	my $r = shift;

	print SOURCE "\t${table}" . "['\\r'][$s] = $r;\n";
	print SOURCE "\t${table}" . "['\\n'][$s] = $r;\n";
}

sub noncrlf_space()
{
	my $table = shift;
	my $s = shift;
	my $r = shift;

	print SOURCE "\t${table}" . "[' '][$s] = $r;\n";
	print SOURCE "\t${table}" . "['\\t'][$s] = $r;\n";
}

sub http_cr()
{
	my $table = shift;
	my $s = shift;
	my $r = shift;

	print SOURCE "\t${table}" . "['\\r'][$s] = $r;\n";	
}

sub http_lf()
{
	my $table = shift;
	my $s = shift;
	my $r = shift;

	print SOURCE "\t${table}" . "['\\n'][$s] = $r;\n";
}

sub http_tab_space()
{
	my $table = shift;
	my $s = shift;
	my $r = shift;

	print SOURCE "\t${table}" . "[' '][$s] = $r;\n";
	print SOURCE "\t${table}" . "['\\t'][$s] = $r;\n";
}

sub http_alpha()
{
	my $table = shift;
	my $s = shift;
	my $r = shift;

	print SOURCE "\tfor (i = 65; i < 91; i++) {\n";
	print SOURCE "\t\t${table}" . "[i][$s] = $r;\n";
	print SOURCE "\t}\n\n";
	
        print SOURCE "\tfor (i = 97; i < 123; i++) {\n";
	print SOURCE "\t\t${table}" . "[i][$s] = $r;\n";
	print SOURCE "\t}\n\n";
}

sub http_digit()
{
	my $table = shift;
	my $s = shift;
	my $r = shift;

	print SOURCE "\tfor (i = 48; i < 58; i++) {\n";
	print SOURCE "\t\t${table}" . "[i][$s] = $r;\n";
	print SOURCE "\t}\n\n";
}

sub http_prnchar()
{
	my $table = shift;
	my $s = shift;
	my $r = shift;

	print SOURCE "\tfor (i = 32; i < 127; i++) {\n";
	print SOURCE "\t\t${table}" . "[i][$s] = $r;\n";
	print SOURCE "\t}\n\n";
}

sub http_extchar()
{
	my $table = shift;
	my $s = shift;
	my $r = shift;

	print SOURCE "\tfor (i = 128; i < MAX_STATE_ROWS; i++) {\n";
	print SOURCE "\t\t${table}" . "[i][$s] = $r;\n";
	print SOURCE "\t}\n\n";
}

sub http_ctlchar()
{
	my $table = shift;
	my $s = shift;
	my $r = shift;

	print SOURCE "\tfor (i = 0; i < 32; i++) {\n";
	print SOURCE "\t\t${table}" . "[i][$s] = $r;\n";
	print SOURCE "\t}\n\n";
	print SOURCE "\t\t${table}" . "[127][$s] = $r;\n";
}

sub http_escchar()
{
	my $table = shift;
	my $s = shift;
	my $r = shift;

	print SOURCE "\tfor (i = 27; i < 27; i++) {\n";
	print SOURCE "\t\t${table}" . "[i][$s] = $r;\n";
	print SOURCE "\t}\n\n";
}

sub is_global()
{
	my $name = shift;

	foreach $a (@$global_states) {
		if ($a eq $name) {
			return 1;
		}
	}

	return 0;
}

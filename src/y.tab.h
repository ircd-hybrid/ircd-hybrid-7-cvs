/* A Bison parser, made by GNU Bison 2.0.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */
/* $Id: y.tab.h,v 7.26 2005/07/24 00:05:45 michael Exp $ */
/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     ACCEPT_PASSWORD = 258,
     ACTION = 259,
     ADMIN = 260,
     AFTYPE = 261,
     T_ALLOW = 262,
     ANTI_NICK_FLOOD = 263,
     ANTI_SPAM_EXIT_MESSAGE_TIME = 264,
     IRCD_AUTH = 265,
     AUTOCONN = 266,
     T_BLOCK = 267,
     BURST_AWAY = 268,
     BYTES = 269,
     KBYTES = 270,
     MBYTES = 271,
     GBYTES = 272,
     TBYTES = 273,
     CALLER_ID_WAIT = 274,
     CAN_FLOOD = 275,
     CAN_IDLE = 276,
     CHANNEL = 277,
     CIPHER_PREFERENCE = 278,
     CLASS = 279,
     COMPRESSED = 280,
     COMPRESSION_LEVEL = 281,
     CONNECT = 282,
     CONNECTFREQ = 283,
     CRYPTLINK = 284,
     DEFAULT_CIPHER_PREFERENCE = 285,
     DEFAULT_FLOODCOUNT = 286,
     DEFAULT_SPLIT_SERVER_COUNT = 287,
     DEFAULT_SPLIT_USER_COUNT = 288,
     DENY = 289,
     DESCRIPTION = 290,
     DIE = 291,
     DISABLE_AUTH = 292,
     DISABLE_HIDDEN = 293,
     DISABLE_LOCAL_CHANNELS = 294,
     DISABLE_REMOTE_COMMANDS = 295,
     DOT_IN_IP6_ADDR = 296,
     DOTS_IN_IDENT = 297,
     DURATION = 298,
     EGDPOOL_PATH = 299,
     EMAIL = 300,
     ENABLE = 301,
     ENCRYPTED = 302,
     EXCEED_LIMIT = 303,
     EXEMPT = 304,
     FAILED_OPER_NOTICE = 305,
     FAKENAME = 306,
     IRCD_FLAGS = 307,
     FLATTEN_LINKS = 308,
     FFAILED_OPERLOG = 309,
     FOPERLOG = 310,
     FUSERLOG = 311,
     GECOS = 312,
     GENERAL = 313,
     GLINE = 314,
     GLINES = 315,
     GLINE_EXEMPT = 316,
     GLINE_LOG = 317,
     GLINE_TIME = 318,
     GLINE_MIN_CIDR = 319,
     GLINE_MIN_CIDR6 = 320,
     GLOBAL_KILL = 321,
     NEED_IDENT = 322,
     HAVENT_READ_CONF = 323,
     HIDDEN = 324,
     HIDDEN_ADMIN = 325,
     HIDDEN_OPER = 326,
     HIDE_SERVER_IPS = 327,
     HIDE_SERVERS = 328,
     HIDE_SPOOF_IPS = 329,
     HOST = 330,
     HUB = 331,
     HUB_MASK = 332,
     IDLETIME = 333,
     IGNORE_BOGUS_TS = 334,
     IP = 335,
     KILL = 336,
     KILL_CHASE_TIME_LIMIT = 337,
     KLINE = 338,
     KLINE_EXEMPT = 339,
     KLINE_REASON = 340,
     KLINE_WITH_REASON = 341,
     KNOCK_DELAY = 342,
     KNOCK_DELAY_CHANNEL = 343,
     LAZYLINK = 344,
     LEAF_MASK = 345,
     LINKS_DELAY = 346,
     LISTEN = 347,
     LOGGING = 348,
     LOG_LEVEL = 349,
     MAXIMUM_LINKS = 350,
     MAX_ACCEPT = 351,
     MAX_BANS = 352,
     MAX_CHANS_PER_USER = 353,
     MAX_GLOBAL = 354,
     MAX_IDENT = 355,
     MAX_LOCAL = 356,
     MAX_NICK_CHANGES = 357,
     MAX_NICK_TIME = 358,
     MAX_NUMBER = 359,
     MAX_TARGETS = 360,
     MESSAGE_LOCALE = 361,
     MIN_NONWILDCARD = 362,
     MIN_NONWILDCARD_SIMPLE = 363,
     MODULE = 364,
     MODULES = 365,
     NAME = 366,
     NEED_PASSWORD = 367,
     NETWORK_DESC = 368,
     NETWORK_NAME = 369,
     NICK = 370,
     NICK_CHANGES = 371,
     NO_CREATE_ON_SPLIT = 372,
     NO_JOIN_ON_SPLIT = 373,
     NO_OPER_FLOOD = 374,
     NO_TILDE = 375,
     NOT = 376,
     NUMBER = 377,
     NUMBER_PER_IDENT = 378,
     NUMBER_PER_IP = 379,
     NUMBER_PER_IP_GLOBAL = 380,
     OPERATOR = 381,
     OPER_LOG = 382,
     OPER_ONLY_UMODES = 383,
     OPER_PASS_RESV = 384,
     OPER_SPY_T = 385,
     OPER_UMODES = 386,
     INVITE_OPS_ONLY = 387,
     PACE_WAIT = 388,
     PACE_WAIT_SIMPLE = 389,
     PASSWORD = 390,
     PATH = 391,
     PING_COOKIE = 392,
     PING_TIME = 393,
     PORT = 394,
     QSTRING = 395,
     QUIET_ON_BAN = 396,
     REASON = 397,
     REDIRPORT = 398,
     REDIRSERV = 399,
     REGEX_T = 400,
     REHASH = 401,
     REMOTE = 402,
     REMOTEBAN = 403,
     RESTRICTED = 404,
     RSA_PRIVATE_KEY_FILE = 405,
     RSA_PUBLIC_KEY_FILE = 406,
     SSL_CERTIFICATE_FILE = 407,
     RESV = 408,
     RESV_EXEMPT = 409,
     SECONDS = 410,
     MINUTES = 411,
     HOURS = 412,
     DAYS = 413,
     WEEKS = 414,
     SENDQ = 415,
     SEND_PASSWORD = 416,
     SERVERHIDE = 417,
     SERVERINFO = 418,
     SERVLINK_PATH = 419,
     SID = 420,
     TKLINE_EXPIRE_NOTICES = 421,
     T_SHARED = 422,
     T_CLUSTER = 423,
     TYPE = 424,
     SHORT_MOTD = 425,
     SILENT = 426,
     SPOOF = 427,
     SPOOF_NOTICE = 428,
     STATS_I_OPER_ONLY = 429,
     STATS_K_OPER_ONLY = 430,
     STATS_O_OPER_ONLY = 431,
     STATS_P_OPER_ONLY = 432,
     TBOOL = 433,
     TMASKED = 434,
     T_REJECT = 435,
     TS_MAX_DELTA = 436,
     TS_WARN_DELTA = 437,
     TWODOTS = 438,
     T_ALL = 439,
     T_BOTS = 440,
     T_SOFTCALLERID = 441,
     T_CALLERID = 442,
     T_CCONN = 443,
     T_CLIENT_FLOOD = 444,
     T_DEBUG = 445,
     T_DRONE = 446,
     T_EXTERNAL = 447,
     T_FULL = 448,
     T_INVISIBLE = 449,
     T_IPV4 = 450,
     T_IPV6 = 451,
     T_LOCOPS = 452,
     T_LOGPATH = 453,
     T_L_CRIT = 454,
     T_L_DEBUG = 455,
     T_L_ERROR = 456,
     T_L_INFO = 457,
     T_L_NOTICE = 458,
     T_L_TRACE = 459,
     T_L_WARN = 460,
     T_MAX_CLIENTS = 461,
     T_NCHANGE = 462,
     T_OPERWALL = 463,
     T_REJ = 464,
     T_SERVNOTICE = 465,
     T_SKILL = 466,
     T_SPY = 467,
     T_SSL = 468,
     T_UNAUTH = 469,
     T_UNRESV = 470,
     T_UNXLINE = 471,
     T_WALLOP = 472,
     THROTTLE_TIME = 473,
     TRUE_NO_OPER_FLOOD = 474,
     UNKLINE = 475,
     USER = 476,
     USE_EGD = 477,
     USE_EXCEPT = 478,
     USE_INVEX = 479,
     USE_KNOCK = 480,
     USE_LOGGING = 481,
     USE_WHOIS_ACTUALLY = 482,
     VHOST = 483,
     VHOST6 = 484,
     XLINE = 485,
     WARN = 486,
     WARN_NO_NLINE = 487
   };
#endif
#define ACCEPT_PASSWORD 258
#define ACTION 259
#define ADMIN 260
#define AFTYPE 261
#define T_ALLOW 262
#define ANTI_NICK_FLOOD 263
#define ANTI_SPAM_EXIT_MESSAGE_TIME 264
#define IRCD_AUTH 265
#define AUTOCONN 266
#define T_BLOCK 267
#define BURST_AWAY 268
#define BYTES 269
#define KBYTES 270
#define MBYTES 271
#define GBYTES 272
#define TBYTES 273
#define CALLER_ID_WAIT 274
#define CAN_FLOOD 275
#define CAN_IDLE 276
#define CHANNEL 277
#define CIPHER_PREFERENCE 278
#define CLASS 279
#define COMPRESSED 280
#define COMPRESSION_LEVEL 281
#define CONNECT 282
#define CONNECTFREQ 283
#define CRYPTLINK 284
#define DEFAULT_CIPHER_PREFERENCE 285
#define DEFAULT_FLOODCOUNT 286
#define DEFAULT_SPLIT_SERVER_COUNT 287
#define DEFAULT_SPLIT_USER_COUNT 288
#define DENY 289
#define DESCRIPTION 290
#define DIE 291
#define DISABLE_AUTH 292
#define DISABLE_HIDDEN 293
#define DISABLE_LOCAL_CHANNELS 294
#define DISABLE_REMOTE_COMMANDS 295
#define DOT_IN_IP6_ADDR 296
#define DOTS_IN_IDENT 297
#define DURATION 298
#define EGDPOOL_PATH 299
#define EMAIL 300
#define ENABLE 301
#define ENCRYPTED 302
#define EXCEED_LIMIT 303
#define EXEMPT 304
#define FAILED_OPER_NOTICE 305
#define FAKENAME 306
#define IRCD_FLAGS 307
#define FLATTEN_LINKS 308
#define FFAILED_OPERLOG 309
#define FOPERLOG 310
#define FUSERLOG 311
#define GECOS 312
#define GENERAL 313
#define GLINE 314
#define GLINES 315
#define GLINE_EXEMPT 316
#define GLINE_LOG 317
#define GLINE_TIME 318
#define GLINE_MIN_CIDR 319
#define GLINE_MIN_CIDR6 320
#define GLOBAL_KILL 321
#define NEED_IDENT 322
#define HAVENT_READ_CONF 323
#define HIDDEN 324
#define HIDDEN_ADMIN 325
#define HIDDEN_OPER 326
#define HIDE_SERVER_IPS 327
#define HIDE_SERVERS 328
#define HIDE_SPOOF_IPS 329
#define HOST 330
#define HUB 331
#define HUB_MASK 332
#define IDLETIME 333
#define IGNORE_BOGUS_TS 334
#define IP 335
#define KILL 336
#define KILL_CHASE_TIME_LIMIT 337
#define KLINE 338
#define KLINE_EXEMPT 339
#define KLINE_REASON 340
#define KLINE_WITH_REASON 341
#define KNOCK_DELAY 342
#define KNOCK_DELAY_CHANNEL 343
#define LAZYLINK 344
#define LEAF_MASK 345
#define LINKS_DELAY 346
#define LISTEN 347
#define LOGGING 348
#define LOG_LEVEL 349
#define MAXIMUM_LINKS 350
#define MAX_ACCEPT 351
#define MAX_BANS 352
#define MAX_CHANS_PER_USER 353
#define MAX_GLOBAL 354
#define MAX_IDENT 355
#define MAX_LOCAL 356
#define MAX_NICK_CHANGES 357
#define MAX_NICK_TIME 358
#define MAX_NUMBER 359
#define MAX_TARGETS 360
#define MESSAGE_LOCALE 361
#define MIN_NONWILDCARD 362
#define MIN_NONWILDCARD_SIMPLE 363
#define MODULE 364
#define MODULES 365
#define NAME 366
#define NEED_PASSWORD 367
#define NETWORK_DESC 368
#define NETWORK_NAME 369
#define NICK 370
#define NICK_CHANGES 371
#define NO_CREATE_ON_SPLIT 372
#define NO_JOIN_ON_SPLIT 373
#define NO_OPER_FLOOD 374
#define NO_TILDE 375
#define NOT 376
#define NUMBER 377
#define NUMBER_PER_IDENT 378
#define NUMBER_PER_IP 379
#define NUMBER_PER_IP_GLOBAL 380
#define OPERATOR 381
#define OPER_LOG 382
#define OPER_ONLY_UMODES 383
#define OPER_PASS_RESV 384
#define OPER_SPY_T 385
#define OPER_UMODES 386
#define INVITE_OPS_ONLY 387
#define PACE_WAIT 388
#define PACE_WAIT_SIMPLE 389
#define PASSWORD 390
#define PATH 391
#define PING_COOKIE 392
#define PING_TIME 393
#define PORT 394
#define QSTRING 395
#define QUIET_ON_BAN 396
#define REASON 397
#define REDIRPORT 398
#define REDIRSERV 399
#define REGEX_T 400
#define REHASH 401
#define REMOTE 402
#define REMOTEBAN 403
#define RESTRICTED 404
#define RSA_PRIVATE_KEY_FILE 405
#define RSA_PUBLIC_KEY_FILE 406
#define SSL_CERTIFICATE_FILE 407
#define RESV 408
#define RESV_EXEMPT 409
#define SECONDS 410
#define MINUTES 411
#define HOURS 412
#define DAYS 413
#define WEEKS 414
#define SENDQ 415
#define SEND_PASSWORD 416
#define SERVERHIDE 417
#define SERVERINFO 418
#define SERVLINK_PATH 419
#define SID 420
#define TKLINE_EXPIRE_NOTICES 421
#define T_SHARED 422
#define T_CLUSTER 423
#define TYPE 424
#define SHORT_MOTD 425
#define SILENT 426
#define SPOOF 427
#define SPOOF_NOTICE 428
#define STATS_I_OPER_ONLY 429
#define STATS_K_OPER_ONLY 430
#define STATS_O_OPER_ONLY 431
#define STATS_P_OPER_ONLY 432
#define TBOOL 433
#define TMASKED 434
#define T_REJECT 435
#define TS_MAX_DELTA 436
#define TS_WARN_DELTA 437
#define TWODOTS 438
#define T_ALL 439
#define T_BOTS 440
#define T_SOFTCALLERID 441
#define T_CALLERID 442
#define T_CCONN 443
#define T_CLIENT_FLOOD 444
#define T_DEBUG 445
#define T_DRONE 446
#define T_EXTERNAL 447
#define T_FULL 448
#define T_INVISIBLE 449
#define T_IPV4 450
#define T_IPV6 451
#define T_LOCOPS 452
#define T_LOGPATH 453
#define T_L_CRIT 454
#define T_L_DEBUG 455
#define T_L_ERROR 456
#define T_L_INFO 457
#define T_L_NOTICE 458
#define T_L_TRACE 459
#define T_L_WARN 460
#define T_MAX_CLIENTS 461
#define T_NCHANGE 462
#define T_OPERWALL 463
#define T_REJ 464
#define T_SERVNOTICE 465
#define T_SKILL 466
#define T_SPY 467
#define T_SSL 468
#define T_UNAUTH 469
#define T_UNRESV 470
#define T_UNXLINE 471
#define T_WALLOP 472
#define THROTTLE_TIME 473
#define TRUE_NO_OPER_FLOOD 474
#define UNKLINE 475
#define USER 476
#define USE_EGD 477
#define USE_EXCEPT 478
#define USE_INVEX 479
#define USE_KNOCK 480
#define USE_LOGGING 481
#define USE_WHOIS_ACTUALLY 482
#define VHOST 483
#define VHOST6 484
#define XLINE 485
#define WARN 486
#define WARN_NO_NLINE 487




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 134 "ircd_parser.y"
typedef union YYSTYPE {
  int number;
  char *string;
} YYSTYPE;
/* Line 1318 of yacc.c.  */
#line 506 "y.tab.h"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;




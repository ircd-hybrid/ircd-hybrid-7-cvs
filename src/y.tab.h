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
/*  $Id: y.tab.h,v 7.40 2005/08/06 16:30:26 michael Exp $ */
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
     BURST_TOPICWHO = 269,
     BYTES = 270,
     KBYTES = 271,
     MBYTES = 272,
     GBYTES = 273,
     TBYTES = 274,
     CALLER_ID_WAIT = 275,
     CAN_FLOOD = 276,
     CAN_IDLE = 277,
     CHANNEL = 278,
     CIPHER_PREFERENCE = 279,
     CLASS = 280,
     COMPRESSED = 281,
     COMPRESSION_LEVEL = 282,
     CONNECT = 283,
     CONNECTFREQ = 284,
     CRYPTLINK = 285,
     DEFAULT_CIPHER_PREFERENCE = 286,
     DEFAULT_FLOODCOUNT = 287,
     DEFAULT_SPLIT_SERVER_COUNT = 288,
     DEFAULT_SPLIT_USER_COUNT = 289,
     DENY = 290,
     DESCRIPTION = 291,
     DIE = 292,
     DISABLE_AUTH = 293,
     DISABLE_HIDDEN = 294,
     DISABLE_LOCAL_CHANNELS = 295,
     DISABLE_REMOTE_COMMANDS = 296,
     DOT_IN_IP6_ADDR = 297,
     DOTS_IN_IDENT = 298,
     DURATION = 299,
     EGDPOOL_PATH = 300,
     EMAIL = 301,
     ENABLE = 302,
     ENCRYPTED = 303,
     EXCEED_LIMIT = 304,
     EXEMPT = 305,
     FAILED_OPER_NOTICE = 306,
     FAKENAME = 307,
     IRCD_FLAGS = 308,
     FLATTEN_LINKS = 309,
     FFAILED_OPERLOG = 310,
     FOPERLOG = 311,
     FUSERLOG = 312,
     GECOS = 313,
     GENERAL = 314,
     GLINE = 315,
     GLINES = 316,
     GLINE_EXEMPT = 317,
     GLINE_LOG = 318,
     GLINE_TIME = 319,
     GLINE_MIN_CIDR = 320,
     GLINE_MIN_CIDR6 = 321,
     GLOBAL_KILL = 322,
     NEED_IDENT = 323,
     HAVENT_READ_CONF = 324,
     HIDDEN = 325,
     HIDDEN_ADMIN = 326,
     HIDDEN_OPER = 327,
     HIDE_SERVER_IPS = 328,
     HIDE_SERVERS = 329,
     HIDE_SPOOF_IPS = 330,
     HOST = 331,
     HUB = 332,
     HUB_MASK = 333,
     IDLETIME = 334,
     IGNORE_BOGUS_TS = 335,
     IP = 336,
     KILL = 337,
     KILL_CHASE_TIME_LIMIT = 338,
     KLINE = 339,
     KLINE_EXEMPT = 340,
     KLINE_REASON = 341,
     KLINE_WITH_REASON = 342,
     KNOCK_DELAY = 343,
     KNOCK_DELAY_CHANNEL = 344,
     LAZYLINK = 345,
     LEAF_MASK = 346,
     LINKS_DELAY = 347,
     LISTEN = 348,
     LOGGING = 349,
     LOG_LEVEL = 350,
     MAXIMUM_LINKS = 351,
     MAX_ACCEPT = 352,
     MAX_BANS = 353,
     MAX_CHANS_PER_USER = 354,
     MAX_GLOBAL = 355,
     MAX_IDENT = 356,
     MAX_LOCAL = 357,
     MAX_NICK_CHANGES = 358,
     MAX_NICK_TIME = 359,
     MAX_NUMBER = 360,
     MAX_TARGETS = 361,
     MESSAGE_LOCALE = 362,
     MIN_NONWILDCARD = 363,
     MIN_NONWILDCARD_SIMPLE = 364,
     MODULE = 365,
     MODULES = 366,
     NAME = 367,
     NEED_PASSWORD = 368,
     NETWORK_DESC = 369,
     NETWORK_NAME = 370,
     NICK = 371,
     NICK_CHANGES = 372,
     NO_CREATE_ON_SPLIT = 373,
     NO_JOIN_ON_SPLIT = 374,
     NO_OPER_FLOOD = 375,
     NO_TILDE = 376,
     NOT = 377,
     NUMBER = 378,
     NUMBER_PER_IDENT = 379,
     NUMBER_PER_IP = 380,
     NUMBER_PER_IP_GLOBAL = 381,
     OPERATOR = 382,
     OPER_LOG = 383,
     OPER_ONLY_UMODES = 384,
     OPER_PASS_RESV = 385,
     OPER_SPY_T = 386,
     OPER_UMODES = 387,
     INVITE_OPS_ONLY = 388,
     PACE_WAIT = 389,
     PACE_WAIT_SIMPLE = 390,
     PASSWORD = 391,
     PATH = 392,
     PING_COOKIE = 393,
     PING_TIME = 394,
     PORT = 395,
     QSTRING = 396,
     QUIET_ON_BAN = 397,
     REASON = 398,
     REDIRPORT = 399,
     REDIRSERV = 400,
     REGEX_T = 401,
     REHASH = 402,
     REMOTE = 403,
     REMOTEBAN = 404,
     RESTRICTED = 405,
     RSA_PRIVATE_KEY_FILE = 406,
     RSA_PUBLIC_KEY_FILE = 407,
     SSL_CERTIFICATE_FILE = 408,
     RESV = 409,
     RESV_EXEMPT = 410,
     SECONDS = 411,
     MINUTES = 412,
     HOURS = 413,
     DAYS = 414,
     WEEKS = 415,
     SENDQ = 416,
     SEND_PASSWORD = 417,
     SERVERHIDE = 418,
     SERVERINFO = 419,
     SERVLINK_PATH = 420,
     IRCD_SID = 421,
     TKLINE_EXPIRE_NOTICES = 422,
     T_SHARED = 423,
     T_CLUSTER = 424,
     TYPE = 425,
     SHORT_MOTD = 426,
     SILENT = 427,
     SPOOF = 428,
     SPOOF_NOTICE = 429,
     STATS_I_OPER_ONLY = 430,
     STATS_K_OPER_ONLY = 431,
     STATS_O_OPER_ONLY = 432,
     STATS_P_OPER_ONLY = 433,
     TBOOL = 434,
     TMASKED = 435,
     T_REJECT = 436,
     TS_MAX_DELTA = 437,
     TS_WARN_DELTA = 438,
     TWODOTS = 439,
     T_ALL = 440,
     T_BOTS = 441,
     T_SOFTCALLERID = 442,
     T_CALLERID = 443,
     T_CCONN = 444,
     T_CLIENT_FLOOD = 445,
     T_DEAF = 446,
     T_DEBUG = 447,
     T_DRONE = 448,
     T_EXTERNAL = 449,
     T_FULL = 450,
     T_INVISIBLE = 451,
     T_IPV4 = 452,
     T_IPV6 = 453,
     T_LOCOPS = 454,
     T_LOGPATH = 455,
     T_L_CRIT = 456,
     T_L_DEBUG = 457,
     T_L_ERROR = 458,
     T_L_INFO = 459,
     T_L_NOTICE = 460,
     T_L_TRACE = 461,
     T_L_WARN = 462,
     T_MAX_CLIENTS = 463,
     T_NCHANGE = 464,
     T_OPERWALL = 465,
     T_REJ = 466,
     T_SERVNOTICE = 467,
     T_SKILL = 468,
     T_SPY = 469,
     T_SSL = 470,
     T_UNAUTH = 471,
     T_UNRESV = 472,
     T_UNXLINE = 473,
     T_WALLOP = 474,
     THROTTLE_TIME = 475,
     TOPICBURST = 476,
     TRUE_NO_OPER_FLOOD = 477,
     UNKLINE = 478,
     USER = 479,
     USE_EGD = 480,
     USE_EXCEPT = 481,
     USE_INVEX = 482,
     USE_KNOCK = 483,
     USE_LOGGING = 484,
     USE_WHOIS_ACTUALLY = 485,
     VHOST = 486,
     VHOST6 = 487,
     XLINE = 488,
     WARN = 489,
     WARN_NO_NLINE = 490
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
#define BURST_TOPICWHO 269
#define BYTES 270
#define KBYTES 271
#define MBYTES 272
#define GBYTES 273
#define TBYTES 274
#define CALLER_ID_WAIT 275
#define CAN_FLOOD 276
#define CAN_IDLE 277
#define CHANNEL 278
#define CIPHER_PREFERENCE 279
#define CLASS 280
#define COMPRESSED 281
#define COMPRESSION_LEVEL 282
#define CONNECT 283
#define CONNECTFREQ 284
#define CRYPTLINK 285
#define DEFAULT_CIPHER_PREFERENCE 286
#define DEFAULT_FLOODCOUNT 287
#define DEFAULT_SPLIT_SERVER_COUNT 288
#define DEFAULT_SPLIT_USER_COUNT 289
#define DENY 290
#define DESCRIPTION 291
#define DIE 292
#define DISABLE_AUTH 293
#define DISABLE_HIDDEN 294
#define DISABLE_LOCAL_CHANNELS 295
#define DISABLE_REMOTE_COMMANDS 296
#define DOT_IN_IP6_ADDR 297
#define DOTS_IN_IDENT 298
#define DURATION 299
#define EGDPOOL_PATH 300
#define EMAIL 301
#define ENABLE 302
#define ENCRYPTED 303
#define EXCEED_LIMIT 304
#define EXEMPT 305
#define FAILED_OPER_NOTICE 306
#define FAKENAME 307
#define IRCD_FLAGS 308
#define FLATTEN_LINKS 309
#define FFAILED_OPERLOG 310
#define FOPERLOG 311
#define FUSERLOG 312
#define GECOS 313
#define GENERAL 314
#define GLINE 315
#define GLINES 316
#define GLINE_EXEMPT 317
#define GLINE_LOG 318
#define GLINE_TIME 319
#define GLINE_MIN_CIDR 320
#define GLINE_MIN_CIDR6 321
#define GLOBAL_KILL 322
#define NEED_IDENT 323
#define HAVENT_READ_CONF 324
#define HIDDEN 325
#define HIDDEN_ADMIN 326
#define HIDDEN_OPER 327
#define HIDE_SERVER_IPS 328
#define HIDE_SERVERS 329
#define HIDE_SPOOF_IPS 330
#define HOST 331
#define HUB 332
#define HUB_MASK 333
#define IDLETIME 334
#define IGNORE_BOGUS_TS 335
#define IP 336
#define KILL 337
#define KILL_CHASE_TIME_LIMIT 338
#define KLINE 339
#define KLINE_EXEMPT 340
#define KLINE_REASON 341
#define KLINE_WITH_REASON 342
#define KNOCK_DELAY 343
#define KNOCK_DELAY_CHANNEL 344
#define LAZYLINK 345
#define LEAF_MASK 346
#define LINKS_DELAY 347
#define LISTEN 348
#define LOGGING 349
#define LOG_LEVEL 350
#define MAXIMUM_LINKS 351
#define MAX_ACCEPT 352
#define MAX_BANS 353
#define MAX_CHANS_PER_USER 354
#define MAX_GLOBAL 355
#define MAX_IDENT 356
#define MAX_LOCAL 357
#define MAX_NICK_CHANGES 358
#define MAX_NICK_TIME 359
#define MAX_NUMBER 360
#define MAX_TARGETS 361
#define MESSAGE_LOCALE 362
#define MIN_NONWILDCARD 363
#define MIN_NONWILDCARD_SIMPLE 364
#define MODULE 365
#define MODULES 366
#define NAME 367
#define NEED_PASSWORD 368
#define NETWORK_DESC 369
#define NETWORK_NAME 370
#define NICK 371
#define NICK_CHANGES 372
#define NO_CREATE_ON_SPLIT 373
#define NO_JOIN_ON_SPLIT 374
#define NO_OPER_FLOOD 375
#define NO_TILDE 376
#define NOT 377
#define NUMBER 378
#define NUMBER_PER_IDENT 379
#define NUMBER_PER_IP 380
#define NUMBER_PER_IP_GLOBAL 381
#define OPERATOR 382
#define OPER_LOG 383
#define OPER_ONLY_UMODES 384
#define OPER_PASS_RESV 385
#define OPER_SPY_T 386
#define OPER_UMODES 387
#define INVITE_OPS_ONLY 388
#define PACE_WAIT 389
#define PACE_WAIT_SIMPLE 390
#define PASSWORD 391
#define PATH 392
#define PING_COOKIE 393
#define PING_TIME 394
#define PORT 395
#define QSTRING 396
#define QUIET_ON_BAN 397
#define REASON 398
#define REDIRPORT 399
#define REDIRSERV 400
#define REGEX_T 401
#define REHASH 402
#define REMOTE 403
#define REMOTEBAN 404
#define RESTRICTED 405
#define RSA_PRIVATE_KEY_FILE 406
#define RSA_PUBLIC_KEY_FILE 407
#define SSL_CERTIFICATE_FILE 408
#define RESV 409
#define RESV_EXEMPT 410
#define SECONDS 411
#define MINUTES 412
#define HOURS 413
#define DAYS 414
#define WEEKS 415
#define SENDQ 416
#define SEND_PASSWORD 417
#define SERVERHIDE 418
#define SERVERINFO 419
#define SERVLINK_PATH 420
#define IRCD_SID 421
#define TKLINE_EXPIRE_NOTICES 422
#define T_SHARED 423
#define T_CLUSTER 424
#define TYPE 425
#define SHORT_MOTD 426
#define SILENT 427
#define SPOOF 428
#define SPOOF_NOTICE 429
#define STATS_I_OPER_ONLY 430
#define STATS_K_OPER_ONLY 431
#define STATS_O_OPER_ONLY 432
#define STATS_P_OPER_ONLY 433
#define TBOOL 434
#define TMASKED 435
#define T_REJECT 436
#define TS_MAX_DELTA 437
#define TS_WARN_DELTA 438
#define TWODOTS 439
#define T_ALL 440
#define T_BOTS 441
#define T_SOFTCALLERID 442
#define T_CALLERID 443
#define T_CCONN 444
#define T_CLIENT_FLOOD 445
#define T_DEAF 446
#define T_DEBUG 447
#define T_DRONE 448
#define T_EXTERNAL 449
#define T_FULL 450
#define T_INVISIBLE 451
#define T_IPV4 452
#define T_IPV6 453
#define T_LOCOPS 454
#define T_LOGPATH 455
#define T_L_CRIT 456
#define T_L_DEBUG 457
#define T_L_ERROR 458
#define T_L_INFO 459
#define T_L_NOTICE 460
#define T_L_TRACE 461
#define T_L_WARN 462
#define T_MAX_CLIENTS 463
#define T_NCHANGE 464
#define T_OPERWALL 465
#define T_REJ 466
#define T_SERVNOTICE 467
#define T_SKILL 468
#define T_SPY 469
#define T_SSL 470
#define T_UNAUTH 471
#define T_UNRESV 472
#define T_UNXLINE 473
#define T_WALLOP 474
#define THROTTLE_TIME 475
#define TOPICBURST 476
#define TRUE_NO_OPER_FLOOD 477
#define UNKLINE 478
#define USER 479
#define USE_EGD 480
#define USE_EXCEPT 481
#define USE_INVEX 482
#define USE_KNOCK 483
#define USE_LOGGING 484
#define USE_WHOIS_ACTUALLY 485
#define VHOST 486
#define VHOST6 487
#define XLINE 488
#define WARN 489
#define WARN_NO_NLINE 490




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 131 "ircd_parser.y"
typedef union YYSTYPE {
  int number;
  char *string;
} YYSTYPE;
/* Line 1318 of yacc.c.  */
#line 512 "y.tab.h"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;




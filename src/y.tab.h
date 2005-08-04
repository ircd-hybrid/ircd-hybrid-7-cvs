/* $Id: y.tab.h,v 7.38 2005/08/04 23:53:48 metalrock Exp $ */
#ifndef YYERRCODE
#define YYERRCODE 256
#endif

#define ACCEPT_PASSWORD 257
#define ACTION 258
#define ADMIN 259
#define AFTYPE 260
#define T_ALLOW 261
#define ANTI_NICK_FLOOD 262
#define ANTI_SPAM_EXIT_MESSAGE_TIME 263
#define IRCD_AUTH 264
#define AUTOCONN 265
#define T_BLOCK 266
#define BURST_AWAY 267
#define BURST_TOPICWHO 268
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
#define IRCD_SID 420
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
#define T_DEAF 445
#define T_DEBUG 446
#define T_DRONE 447
#define T_EXTERNAL 448
#define T_FULL 449
#define T_INVISIBLE 450
#define T_IPV4 451
#define T_IPV6 452
#define T_LOCOPS 453
#define T_LOGPATH 454
#define T_L_CRIT 455
#define T_L_DEBUG 456
#define T_L_ERROR 457
#define T_L_INFO 458
#define T_L_NOTICE 459
#define T_L_TRACE 460
#define T_L_WARN 461
#define T_MAX_CLIENTS 462
#define T_NCHANGE 463
#define T_OPERWALL 464
#define T_REJ 465
#define T_SERVNOTICE 466
#define T_SKILL 467
#define T_SPY 468
#define T_SSL 469
#define T_UNAUTH 470
#define T_UNRESV 471
#define T_UNXLINE 472
#define T_WALLOP 473
#define THROTTLE_TIME 474
#define TOPICBURST 475
#define TRUE_NO_OPER_FLOOD 476
#define UNKLINE 477
#define USER 478
#define USE_EGD 479
#define USE_EXCEPT 480
#define USE_INVEX 481
#define USE_KNOCK 482
#define USE_LOGGING 483
#define USE_WHOIS_ACTUALLY 484
#define VHOST 485
#define VHOST6 486
#define XLINE 487
#define WARN 488
#define WARN_NO_NLINE 489
typedef union {
  int number;
  char *string;
} YYSTYPE;
extern YYSTYPE yylval;

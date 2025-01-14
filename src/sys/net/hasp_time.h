/* MIT License - Copyright (c) 2019-2023 Francis Van Roie
   For full license information read the LICENSE file in the project folder */

#ifndef HASP_TIME_H
#define HASP_TIME_H

#include "hasplib.h"

/* ===== Default Event Processors ===== */
void timeSetup();

/* ===== Special Event Processors ===== */

/* ===== Getter and Setter Functions ===== */
String time_zone_to_possix(const char* timezone);

/* ===== Read/Write Configuration ===== */
#if HASP_USE_CONFIG > 0
bool timeGetConfig(const JsonObject& settings);
bool timeSetConfig(const JsonObject& settings);
#endif

#ifndef MYTZ
#define MYTZ "EST5EDT,M3.2.0/2,M11.1.0"
#endif

#ifndef TIMEZONE
#define TIMEZONE "Etc/GMT0"
#endif

#ifndef NTPSERVER1
#define NTPSERVER1 "pool.ntp.org"
#endif

#ifndef NTPSERVER2
#define NTPSERVER2 "time.nist.gov"
#endif

#ifndef NTPSERVER3
#define NTPSERVER3 "time.google.com"
#endif

#define TZ_AFRICA_ABIDJAN 59846                 // Africa/Abidjan
#define TZ_AFRICA_ACCRA 46363                   // Africa/Accra
#define TZ_AFRICA_ADDIS_ABABA 22902             // Africa/Addis_Ababa
#define TZ_AFRICA_ALGIERS 49828                 // Africa/Algiers
#define TZ_AFRICA_ASMARA 42538                  // Africa/Asmara
#define TZ_AFRICA_BAMAKO 22444                  // Africa/Bamako
#define TZ_AFRICA_BANGUI 34785                  // Africa/Bangui
#define TZ_AFRICA_BANJUL 46695                  // Africa/Banjul
#define TZ_AFRICA_BISSAU 18640                  // Africa/Bissau
#define TZ_AFRICA_BLANTYRE 54148                // Africa/Blantyre
#define TZ_AFRICA_BRAZZAVILLE 53101             // Africa/Brazzaville
#define TZ_AFRICA_BUJUMBURA 12380               // Africa/Bujumbura
#define TZ_AFRICA_CAIRO 11955                   // Africa/Cairo
#define TZ_AFRICA_CASABLANCA 27318              // Africa/Casablanca
#define TZ_AFRICA_CEUTA 11307                   // Africa/Ceuta
#define TZ_AFRICA_CONAKRY 13182                 // Africa/Conakry
#define TZ_AFRICA_DAKAR 43146                   // Africa/Dakar
#define TZ_AFRICA_DAR_ES_SALAAM 39585           // Africa/Dar_es_Salaam
#define TZ_AFRICA_DJIBOUTI 40623                // Africa/Djibouti
#define TZ_AFRICA_DOUALA 53857                  // Africa/Douala
#define TZ_AFRICA_EL_AAIUN 56767                // Africa/El_Aaiun
#define TZ_AFRICA_FREETOWN 50611                // Africa/Freetown
#define TZ_AFRICA_GABORONE 36720                // Africa/Gaborone
#define TZ_AFRICA_HARARE 46224                  // Africa/Harare
#define TZ_AFRICA_JOHANNESBURG 27401            // Africa/Johannesburg
#define TZ_AFRICA_JUBA 59967                    // Africa/Juba
#define TZ_AFRICA_KAMPALA 38312                 // Africa/Kampala
#define TZ_AFRICA_KHARTOUM 15094                // Africa/Khartoum
#define TZ_AFRICA_KIGALI 54954                  // Africa/Kigali
#define TZ_AFRICA_KINSHASA 54815                // Africa/Kinshasa
#define TZ_AFRICA_LAGOS 26113                   // Africa/Lagos
#define TZ_AFRICA_LIBREVILLE 31439              // Africa/Libreville
#define TZ_AFRICA_LOME 12656                    // Africa/Lome
#define TZ_AFRICA_LUANDA 36616                  // Africa/Luanda
#define TZ_AFRICA_LUBUMBASHI 45529              // Africa/Lubumbashi
#define TZ_AFRICA_LUSAKA 29858                  // Africa/Lusaka
#define TZ_AFRICA_MALABO 45675                  // Africa/Malabo
#define TZ_AFRICA_MAPUTO 12265                  // Africa/Maputo
#define TZ_AFRICA_MASERU 43422                  // Africa/Maseru
#define TZ_AFRICA_MBABANE 62957                 // Africa/Mbabane
#define TZ_AFRICA_MOGADISHU 2122                // Africa/Mogadishu
#define TZ_AFRICA_MONROVIA 27130                // Africa/Monrovia
#define TZ_AFRICA_NAIROBI 23877                 // Africa/Nairobi
#define TZ_AFRICA_NDJAMENA 24653                // Africa/Ndjamena
#define TZ_AFRICA_NIAMEY 52816                  // Africa/Niamey
#define TZ_AFRICA_NOUAKCHOTT 26657              // Africa/Nouakchott
#define TZ_AFRICA_OUAGADOUGOU 4323              // Africa/Ouagadougou
#define TZ_AFRICA_PORTO__NOVO 60078             // Africa/Porto-Novo
#define TZ_AFRICA_SAO_TOME 34246                // Africa/Sao_Tome
#define TZ_AFRICA_TRIPOLI 51826                 // Africa/Tripoli
#define TZ_AFRICA_TUNIS 6146                    // Africa/Tunis
#define TZ_AFRICA_WINDHOEK 28586                // Africa/Windhoek
#define TZ_AMERICA_ADAK 1518                    // America/Adak
#define TZ_AMERICA_ANCHORAGE 28265              // America/Anchorage
#define TZ_AMERICA_ANGUILLA 59956               // America/Anguilla
#define TZ_AMERICA_ANTIGUA 24240                // America/Antigua
#define TZ_AMERICA_ARAGUAINA 53816              // America/Araguaina
#define TZ_AMERICA_ARGENTINA_BUENOS_AIRES 32194 // America/Argentina/Buenos_Aires
#define TZ_AMERICA_ARGENTINA_CATAMARCA 41352    // America/Argentina/Catamarca
#define TZ_AMERICA_ARGENTINA_CORDOBA 62119      // America/Argentina/Cordoba
#define TZ_AMERICA_ARGENTINA_JUJUY 29658        // America/Argentina/Jujuy
#define TZ_AMERICA_ARGENTINA_LA_RIOJA 52686     // America/Argentina/La_Rioja
#define TZ_AMERICA_ARGENTINA_MENDOZA 287        // America/Argentina/Mendoza
#define TZ_AMERICA_ARGENTINA_RIO_GALLEGOS 17200 // America/Argentina/Rio_Gallegos
#define TZ_AMERICA_ARGENTINA_SALTA 39586        // America/Argentina/Salta
#define TZ_AMERICA_ARGENTINA_SAN_JUAN 31552     // America/Argentina/San_Juan
#define TZ_AMERICA_ARGENTINA_SAN_LUIS 7867      // America/Argentina/San_Luis
#define TZ_AMERICA_ARGENTINA_TUCUMAN 15646      // America/Argentina/Tucuman
#define TZ_AMERICA_ARGENTINA_USHUAIA 35845      // America/Argentina/Ushuaia
#define TZ_AMERICA_ARUBA 5186                   // America/Aruba
#define TZ_AMERICA_ASUNCION 14577               // America/Asuncion
#define TZ_AMERICA_ATIKOKAN 24575               // America/Atikokan
#define TZ_AMERICA_BAHIA 52928                  // America/Bahia
#define TZ_AMERICA_BAHIA_BANDERAS 36403         // America/Bahia_Banderas
#define TZ_AMERICA_BARBADOS 40663               // America/Barbados
#define TZ_AMERICA_BELEM 20176                  // America/Belem
#define TZ_AMERICA_BELIZE 42700                 // America/Belize
#define TZ_AMERICA_BLANC__SABLON 51769          // America/Blanc-Sablon
#define TZ_AMERICA_BOA_VISTA 62081              // America/Boa_Vista
#define TZ_AMERICA_BOGOTA 42147                 // America/Bogota
#define TZ_AMERICA_BOISE 19245                  // America/Boise
#define TZ_AMERICA_CAMBRIDGE_BAY 59826          // America/Cambridge_Bay
#define TZ_AMERICA_CAMPO_GRANDE 5419            // America/Campo_Grande
#define TZ_AMERICA_CANCUN 53069                 // America/Cancun
#define TZ_AMERICA_CARACAS 50343                // America/Caracas
#define TZ_AMERICA_CAYENNE 20058                // America/Cayenne
#define TZ_AMERICA_CAYMAN 23968                 // America/Cayman
#define TZ_AMERICA_CHICAGO 39497                // America/Chicago
#define TZ_AMERICA_CHIHUAHUA 9283               // America/Chihuahua
#define TZ_AMERICA_COSTA_RICA 21857             // America/Costa_Rica
#define TZ_AMERICA_CRESTON 7989                 // America/Creston
#define TZ_AMERICA_CUIABA 1002                  // America/Cuiaba
#define TZ_AMERICA_CURACAO 23951                // America/Curacao
#define TZ_AMERICA_DANMARKSHAVN 65383           // America/Danmarkshavn
#define TZ_AMERICA_DAWSON 32217                 // America/Dawson
#define TZ_AMERICA_DAWSON_CREEK 10326           // America/Dawson_Creek
#define TZ_AMERICA_DENVER 53047                 // America/Denver
#define TZ_AMERICA_DETROIT 36570                // America/Detroit
#define TZ_AMERICA_DOMINICA 42081               // America/Dominica
#define TZ_AMERICA_EDMONTON 65063               // America/Edmonton
#define TZ_AMERICA_EIRUNEPE 56980               // America/Eirunepe
#define TZ_AMERICA_EL_SALVADOR 50933            // America/El_Salvador
#define TZ_AMERICA_FORTALEZA 35541              // America/Fortaleza
#define TZ_AMERICA_FORT_NELSON 7728             // America/Fort_Nelson
#define TZ_AMERICA_GLACE_BAY 38296              // America/Glace_Bay
#define TZ_AMERICA_GODTHAB 27056                // America/Godthab
#define TZ_AMERICA_GOOSE_BAY 51667              // America/Goose_Bay
#define TZ_AMERICA_GRAND_TURK 1262              // America/Grand_Turk
#define TZ_AMERICA_GRENADA 39977                // America/Grenada
#define TZ_AMERICA_GUADELOUPE 3572              // America/Guadeloupe
#define TZ_AMERICA_GUATEMALA 43020              // America/Guatemala
#define TZ_AMERICA_GUAYAQUIL 35489              // America/Guayaquil
#define TZ_AMERICA_GUYANA 38858                 // America/Guyana
#define TZ_AMERICA_HALIFAX 59302                // America/Halifax
#define TZ_AMERICA_HAVANA 6840                  // America/Havana
#define TZ_AMERICA_HERMOSILLO 5347              // America/Hermosillo
#define TZ_AMERICA_INDIANA_INDIANAPOLIS 22259   // America/Indiana/Indianapolis
#define TZ_AMERICA_INDIANA_KNOX 13714           // America/Indiana/Knox
#define TZ_AMERICA_INDIANA_MARENGO 59081        // America/Indiana/Marengo
#define TZ_AMERICA_INDIANA_PETERSBURG 46069     // America/Indiana/Petersburg
#define TZ_AMERICA_INDIANA_TELL_CITY 2291       // America/Indiana/Tell_City
#define TZ_AMERICA_INDIANA_VEVAY 21657          // America/Indiana/Vevay
#define TZ_AMERICA_INDIANA_VINCENNES 14789      // America/Indiana/Vincennes
#define TZ_AMERICA_INDIANA_WINAMAC 29124        // America/Indiana/Winamac
#define TZ_AMERICA_INUVIK 49833                 // America/Inuvik
#define TZ_AMERICA_IQALUIT 12748                // America/Iqaluit
#define TZ_AMERICA_JAMAICA 52379                // America/Jamaica
#define TZ_AMERICA_JUNEAU 65079                 // America/Juneau
#define TZ_AMERICA_KENTUCKY_LOUISVILLE 31752    // America/Kentucky/Louisville
#define TZ_AMERICA_KENTUCKY_MONTICELLO 34078    // America/Kentucky/Monticello
#define TZ_AMERICA_KRALENDIJK 23362             // America/Kralendijk
#define TZ_AMERICA_LA_PAZ 50176                 // America/La_Paz
#define TZ_AMERICA_LIMA 20114                   // America/Lima
#define TZ_AMERICA_LOS_ANGELES 36703            // America/Los_Angeles
#define TZ_AMERICA_LOWER_PRINCES 29931          // America/Lower_Princes
#define TZ_AMERICA_MACEIO 49053                 // America/Maceio
#define TZ_AMERICA_MANAGUA 56459                // America/Managua
#define TZ_AMERICA_MANAUS 31942                 // America/Manaus
#define TZ_AMERICA_MARIGOT 56608                // America/Marigot
#define TZ_AMERICA_MARTINIQUE 39636             // America/Martinique
#define TZ_AMERICA_MATAMOROS 22258              // America/Matamoros
#define TZ_AMERICA_MAZATLAN 58305               // America/Mazatlan
#define TZ_AMERICA_MENOMINEE 45912              // America/Menominee
#define TZ_AMERICA_MERIDA 45965                 // America/Merida
#define TZ_AMERICA_METLAKATLA 419               // America/Metlakatla
#define TZ_AMERICA_MEXICO_CITY 59348            // America/Mexico_City
#define TZ_AMERICA_MIQUELON 8423                // America/Miquelon
#define TZ_AMERICA_MONCTON 38811                // America/Moncton
#define TZ_AMERICA_MONTERREY 10192              // America/Monterrey
#define TZ_AMERICA_MONTEVIDEO 56703             // America/Montevideo
#define TZ_AMERICA_MONTREAL 24295               // America/Montreal
#define TZ_AMERICA_MONTSERRAT 3758              // America/Montserrat
#define TZ_AMERICA_NASSAU 1000                  // America/Nassau
#define TZ_AMERICA_NEW_YORK 46127               // America/New_York
#define TZ_AMERICA_NIPIGON 6033                 // America/Nipigon
#define TZ_AMERICA_NOME 19738                   // America/Nome
#define TZ_AMERICA_NORONHA 27720                // America/Noronha
#define TZ_AMERICA_NORTH_DAKOTA_BEULAH 39271    // America/North_Dakota/Beulah
#define TZ_AMERICA_NORTH_DAKOTA_CENTER 49787    // America/North_Dakota/Center
#define TZ_AMERICA_NORTH_DAKOTA_NEW_SALEM 38209 // America/North_Dakota/New_Salem
#define TZ_AMERICA_NUUK 44062                   // America/Nuuk
#define TZ_AMERICA_OJINAGA 33530                // America/Ojinaga
#define TZ_AMERICA_PANAMA 40569                 // America/Panama
#define TZ_AMERICA_PANGNIRTUNG 4646             // America/Pangnirtung
#define TZ_AMERICA_PARAMARIBO 46585             // America/Paramaribo
#define TZ_AMERICA_PHOENIX 23662                // America/Phoenix
#define TZ_AMERICA_PORT__AU__PRINCE 25047       // America/Port-au-Prince
#define TZ_AMERICA_PORT_OF_SPAIN 63934          // America/Port_of_Spain
#define TZ_AMERICA_PORTO_VELHO 34354            // America/Porto_Velho
#define TZ_AMERICA_PUERTO_RICO 42868            // America/Puerto_Rico
#define TZ_AMERICA_PUNTA_ARENAS 37782           // America/Punta_Arenas
#define TZ_AMERICA_RAINY_RIVER 23441            // America/Rainy_River
#define TZ_AMERICA_RANKIN_INLET 41897           // America/Rankin_Inlet
#define TZ_AMERICA_RECIFE 24345                 // America/Recife
#define TZ_AMERICA_REGINA 41993                 // America/Regina
#define TZ_AMERICA_RESOLUTE 43530               // America/Resolute
#define TZ_AMERICA_RIO_BRANCO 51889             // America/Rio_Branco
#define TZ_AMERICA_SANTAREM 27694               // America/Santarem
#define TZ_AMERICA_SANTIAGO 60181               // America/Santiago
#define TZ_AMERICA_SANTO_DOMINGO 59690          // America/Santo_Domingo
#define TZ_AMERICA_SAO_PAULO 16456              // America/Sao_Paulo
#define TZ_AMERICA_SCORESBYSUND 23665           // America/Scoresbysund
#define TZ_AMERICA_SITKA 24147                  // America/Sitka
#define TZ_AMERICA_ST_BARTHELEMY 5888           // America/St_Barthelemy
#define TZ_AMERICA_ST_JOHNS 34987               // America/St_Johns
#define TZ_AMERICA_ST_KITTS 48824               // America/St_Kitts
#define TZ_AMERICA_ST_LUCIA 56405               // America/St_Lucia
#define TZ_AMERICA_ST_THOMAS 30529              // America/St_Thomas
#define TZ_AMERICA_ST_VINCENT 19974             // America/St_Vincent
#define TZ_AMERICA_SWIFT_CURRENT 39308          // America/Swift_Current
#define TZ_AMERICA_TEGUCIGALPA 31165            // America/Tegucigalpa
#define TZ_AMERICA_THULE 64601                  // America/Thule
#define TZ_AMERICA_THUNDER_BAY 34494            // America/Thunder_Bay
#define TZ_AMERICA_TIJUANA 57395                // America/Tijuana
#define TZ_AMERICA_TORONTO 1616                 // America/Toronto
#define TZ_AMERICA_TORTOLA 10118                // America/Tortola
#define TZ_AMERICA_VANCOUVER 3708               // America/Vancouver
#define TZ_AMERICA_WHITEHORSE 22549             // America/Whitehorse
#define TZ_AMERICA_WINNIPEG 54300               // America/Winnipeg
#define TZ_AMERICA_YAKUTAT 308                  // America/Yakutat
#define TZ_AMERICA_YELLOWKNIFE 22480            // America/Yellowknife
#define TZ_ANTARCTICA_CASEY 37368               // Antarctica/Casey
#define TZ_ANTARCTICA_DAVIS 8306                // Antarctica/Davis
#define TZ_ANTARCTICA_DUMONTDURVILLE 25979      // Antarctica/DumontDUrville
#define TZ_ANTARCTICA_MACQUARIE 19183           // Antarctica/Macquarie
#define TZ_ANTARCTICA_MAWSON 53888              // Antarctica/Mawson
#define TZ_ANTARCTICA_MCMURDO 42990             // Antarctica/McMurdo
#define TZ_ANTARCTICA_PALMER 40592              // Antarctica/Palmer
#define TZ_ANTARCTICA_ROTHERA 6290              // Antarctica/Rothera
#define TZ_ANTARCTICA_SYOWA 55874               // Antarctica/Syowa
#define TZ_ANTARCTICA_TROLL 33120               // Antarctica/Troll
#define TZ_ANTARCTICA_VOSTOK 15431              // Antarctica/Vostok
#define TZ_ARCTIC_LONGYEARBYEN 25388            // Arctic/Longyearbyen
#define TZ_ASIA_ADEN 13361                      // Asia/Aden
#define TZ_ASIA_ALMATY 64809                    // Asia/Almaty
#define TZ_ASIA_AMMAN 43017                     // Asia/Amman
#define TZ_ASIA_ANADYR 8686                     // Asia/Anadyr
#define TZ_ASIA_AQTAU 22419                     // Asia/Aqtau
#define TZ_ASIA_AQTOBE 25075                    // Asia/Aqtobe
#define TZ_ASIA_ASHGABAT 1930                   // Asia/Ashgabat
#define TZ_ASIA_ATYRAU 49413                    // Asia/Atyrau
#define TZ_ASIA_BAGHDAD 35714                   // Asia/Baghdad
#define TZ_ASIA_BAHRAIN 58744                   // Asia/Bahrain
#define TZ_ASIA_BAKU 55278                      // Asia/Baku
#define TZ_ASIA_BANGKOK 50122                   // Asia/Bangkok
#define TZ_ASIA_BARNAUL 23416                   // Asia/Barnaul
#define TZ_ASIA_BEIRUT 35376                    // Asia/Beirut
#define TZ_ASIA_BISHKEK 30480                   // Asia/Bishkek
#define TZ_ASIA_BRUNEI 58418                    // Asia/Brunei
#define TZ_ASIA_CHITA 6380                      // Asia/Chita
#define TZ_ASIA_CHOIBALSAN 63863                // Asia/Choibalsan
#define TZ_ASIA_COLOMBO 59270                   // Asia/Colombo
#define TZ_ASIA_DAMASCUS 50628                  // Asia/Damascus
#define TZ_ASIA_DHAKA 63918                     // Asia/Dhaka
#define TZ_ASIA_DILI 62887                      // Asia/Dili
#define TZ_ASIA_DUBAI 41076                     // Asia/Dubai
#define TZ_ASIA_DUSHANBE 16571                  // Asia/Dushanbe
#define TZ_ASIA_FAMAGUSTA 16958                 // Asia/Famagusta
#define TZ_ASIA_GAZA 61254                      // Asia/Gaza
#define TZ_ASIA_HEBRON 6897                     // Asia/Hebron
#define TZ_ASIA_HO_CHI_MINH 4836                // Asia/Ho_Chi_Minh
#define TZ_ASIA_HONG_KONG 19895                 // Asia/Hong_Kong
#define TZ_ASIA_HOVD 38938                      // Asia/Hovd
#define TZ_ASIA_IRKUTSK 39028                   // Asia/Irkutsk
#define TZ_ASIA_JAKARTA 41997                   // Asia/Jakarta
#define TZ_ASIA_JAYAPURA 9016                   // Asia/Jayapura
#define TZ_ASIA_JERUSALEM 63395                 // Asia/Jerusalem
#define TZ_ASIA_KABUL 61310                     // Asia/Kabul
#define TZ_ASIA_KAMCHATKA 55328                 // Asia/Kamchatka
#define TZ_ASIA_KARACHI 16410                   // Asia/Karachi
#define TZ_ASIA_KATHMANDU 49212                 // Asia/Kathmandu
#define TZ_ASIA_KHANDYGA 33278                  // Asia/Khandyga
#define TZ_ASIA_KOLKATA 25318                   // Asia/Kolkata
#define TZ_ASIA_KRASNOYARSK 41603               // Asia/Krasnoyarsk
#define TZ_ASIA_KUALA_LUMPUR 33409              // Asia/Kuala_Lumpur
#define TZ_ASIA_KUCHING 23470                   // Asia/Kuching
#define TZ_ASIA_KUWAIT 17764                    // Asia/Kuwait
#define TZ_ASIA_MACAU 47134                     // Asia/Macau
#define TZ_ASIA_MAGADAN 5598                    // Asia/Magadan
#define TZ_ASIA_MAKASSAR 13856                  // Asia/Makassar
#define TZ_ASIA_MANILA 49609                    // Asia/Manila
#define TZ_ASIA_MUSCAT 57840                    // Asia/Muscat
#define TZ_ASIA_NICOSIA 59199                   // Asia/Nicosia
#define TZ_ASIA_NOVOKUZNETSK 29540              // Asia/Novokuznetsk
#define TZ_ASIA_NOVOSIBIRSK 61774               // Asia/Novosibirsk
#define TZ_ASIA_OMSK 11675                      // Asia/Omsk
#define TZ_ASIA_ORAL 30387                      // Asia/Oral
#define TZ_ASIA_PHNOM_PENH 39103                // Asia/Phnom_Penh
#define TZ_ASIA_PONTIANAK 43478                 // Asia/Pontianak
#define TZ_ASIA_PYONGYANG 20443                 // Asia/Pyongyang
#define TZ_ASIA_QATAR 15280                     // Asia/Qatar
#define TZ_ASIA_QYZYLORDA 13248                 // Asia/Qyzylorda
#define TZ_ASIA_RIYADH 6728                     // Asia/Riyadh
#define TZ_ASIA_SAKHALIN 57760                  // Asia/Sakhalin
#define TZ_ASIA_SAMARKAND 48471                 // Asia/Samarkand
#define TZ_ASIA_SEOUL 62479                     // Asia/Seoul
#define TZ_ASIA_SHANGHAI 31024                  // Asia/Shanghai
#define TZ_ASIA_SINGAPORE 8191                  // Asia/Singapore
#define TZ_ASIA_SREDNEKOLYMSK 34938             // Asia/Srednekolymsk
#define TZ_ASIA_TAIPEI 49565                    // Asia/Taipei
#define TZ_ASIA_TASHKENT 22919                  // Asia/Tashkent
#define TZ_ASIA_TBILISI 1225                    // Asia/Tbilisi
#define TZ_ASIA_TEHRAN 35565                    // Asia/Tehran
#define TZ_ASIA_THIMPHU 34528                   // Asia/Thimphu
#define TZ_ASIA_TOKYO 15745                     // Asia/Tokyo
#define TZ_ASIA_TOMSK 23301                     // Asia/Tomsk
#define TZ_ASIA_ULAANBAATAR 57077               // Asia/Ulaanbaatar
#define TZ_ASIA_URUMQI 3922                     // Asia/Urumqi
#define TZ_ASIA_UST__NERA 49026                 // Asia/Ust-Nera
#define TZ_ASIA_VIENTIANE 7682                  // Asia/Vientiane
#define TZ_ASIA_VLADIVOSTOK 38145               // Asia/Vladivostok
#define TZ_ASIA_YAKUTSK 2901                    // Asia/Yakutsk
#define TZ_ASIA_YANGON 12933                    // Asia/Yangon
#define TZ_ASIA_YEKATERINBURG 30663             // Asia/Yekaterinburg
#define TZ_ASIA_YEREVAN 14335                   // Asia/Yerevan
#define TZ_ATLANTIC_AZORES 38417                // Atlantic/Azores
#define TZ_ATLANTIC_BERMUDA 57485               // Atlantic/Bermuda
#define TZ_ATLANTIC_CANARY 15455                // Atlantic/Canary
#define TZ_ATLANTIC_CAPE_VERDE 41471            // Atlantic/Cape_Verde
#define TZ_ATLANTIC_FAROE 51462                 // Atlantic/Faroe
#define TZ_ATLANTIC_MADEIRA 11388               // Atlantic/Madeira
#define TZ_ATLANTIC_REYKJAVIK 58805             // Atlantic/Reykjavik
#define TZ_ATLANTIC_SOUTH_GEORGIA 61829         // Atlantic/South_Georgia
#define TZ_ATLANTIC_STANLEY 24651               // Atlantic/Stanley
#define TZ_ATLANTIC_ST_HELENA 51168             // Atlantic/St_Helena
#define TZ_AUSTRALIA_ADELAIDE 53986             // Australia/Adelaide
#define TZ_AUSTRALIA_BRISBANE 1247              // Australia/Brisbane
#define TZ_AUSTRALIA_BROKEN_HILL 52284          // Australia/Broken_Hill
#define TZ_AUSTRALIA_CURRIE 58525               // Australia/Currie
#define TZ_AUSTRALIA_DARWIN 10070               // Australia/Darwin
#define TZ_AUSTRALIA_EUCLA 25017                // Australia/Eucla
#define TZ_AUSTRALIA_HOBART 32663               // Australia/Hobart
#define TZ_AUSTRALIA_LINDEMAN 4631              // Australia/Lindeman
#define TZ_AUSTRALIA_LORD_HOWE 58896            // Australia/Lord_Howe
#define TZ_AUSTRALIA_MELBOURNE 3878             // Australia/Melbourne
#define TZ_AUSTRALIA_PERTH 21858                // Australia/Perth
#define TZ_AUSTRALIA_SYDNEY 52019               // Australia/Sydney
#define TZ_EUROPE_AMSTERDAM 12329               // Europe/Amsterdam
#define TZ_EUROPE_ANDORRA 64604                 // Europe/Andorra
#define TZ_EUROPE_ASTRAKHAN 37742               // Europe/Astrakhan
#define TZ_EUROPE_ATHENS 13890                  // Europe/Athens
#define TZ_EUROPE_BELGRADE 30043                // Europe/Belgrade
#define TZ_EUROPE_BERLIN 39215                  // Europe/Berlin
#define TZ_EUROPE_BRATISLAVA 16250              // Europe/Bratislava
#define TZ_EUROPE_BRUSSELS 33268                // Europe/Brussels
#define TZ_EUROPE_BUCHAREST 40208               // Europe/Bucharest
#define TZ_EUROPE_BUDAPEST 47539                // Europe/Budapest
#define TZ_EUROPE_BUSINGEN 56952                // Europe/Busingen
#define TZ_EUROPE_CHISINAU 46357                // Europe/Chisinau
#define TZ_EUROPE_COPENHAGEN 27191              // Europe/Copenhagen
#define TZ_EUROPE_DUBLIN 16333                  // Europe/Dublin
#define TZ_EUROPE_GIBRALTAR 56251               // Europe/Gibraltar
#define TZ_EUROPE_GUERNSEY 5857                 // Europe/Guernsey
#define TZ_EUROPE_HELSINKI 34932                // Europe/Helsinki
#define TZ_EUROPE_ISLE_OF_MAN 33537             // Europe/Isle_of_Man
#define TZ_EUROPE_ISTANBUL 64847                // Europe/Istanbul
#define TZ_EUROPE_JERSEY 3773                   // Europe/Jersey
#define TZ_EUROPE_KALININGRAD 47649             // Europe/Kaliningrad
#define TZ_EUROPE_KIEV 25980                    // Europe/Kiev
#define TZ_EUROPE_KIROV 49614                   // Europe/Kirov
#define TZ_EUROPE_LISBON 6360                   // Europe/Lisbon
#define TZ_EUROPE_LJUBLJANA 13118               // Europe/Ljubljana
#define TZ_EUROPE_LONDON 24101                  // Europe/London
#define TZ_EUROPE_LUXEMBOURG 60371              // Europe/Luxembourg
#define TZ_EUROPE_MADRID 15722                  // Europe/Madrid
#define TZ_EUROPE_MALTA 40440                   // Europe/Malta
#define TZ_EUROPE_MARIEHAMN 21511               // Europe/Mariehamn
#define TZ_EUROPE_MINSK 17085                   // Europe/Minsk
#define TZ_EUROPE_MONACO 36334                  // Europe/Monaco
#define TZ_EUROPE_MOSCOW 50087                  // Europe/Moscow
#define TZ_EUROPE_OSLO 17716                    // Europe/Oslo
#define TZ_EUROPE_PARIS 5470                    // Europe/Paris
#define TZ_EUROPE_PODGORICA 53237               // Europe/Podgorica
#define TZ_EUROPE_PRAGUE 38501                  // Europe/Prague
#define TZ_EUROPE_RIGA 6942                     // Europe/Riga
#define TZ_EUROPE_ROME 31138                    // Europe/Rome
#define TZ_EUROPE_SAMARA 54846                  // Europe/Samara
#define TZ_EUROPE_SAN_MARINO 13112              // Europe/San_Marino
#define TZ_EUROPE_SARAJEVO 21246                // Europe/Sarajevo
#define TZ_EUROPE_SARATOV 46897                 // Europe/Saratov
#define TZ_EUROPE_SIMFEROPOL 13991              // Europe/Simferopol
#define TZ_EUROPE_SKOPJE 5217                   // Europe/Skopje
#define TZ_EUROPE_SOFIA 58037                   // Europe/Sofia
#define TZ_EUROPE_STOCKHOLM 63761               // Europe/Stockholm
#define TZ_EUROPE_TALLINN 47375                 // Europe/Tallinn
#define TZ_EUROPE_TIRANE 16968                  // Europe/Tirane
#define TZ_EUROPE_ULYANOVSK 46625               // Europe/Ulyanovsk
#define TZ_EUROPE_UZHGOROD 18985                // Europe/Uzhgorod
#define TZ_EUROPE_VADUZ 31057                   // Europe/Vaduz
#define TZ_EUROPE_VATICAN 48035                 // Europe/Vatican
#define TZ_EUROPE_VIENNA 13468                  // Europe/Vienna
#define TZ_EUROPE_VILNIUS 17221                 // Europe/Vilnius
#define TZ_EUROPE_VOLGOGRAD 19164               // Europe/Volgograd
#define TZ_EUROPE_WARSAW 33262                  // Europe/Warsaw
#define TZ_EUROPE_ZAGREB 40668                  // Europe/Zagreb
#define TZ_EUROPE_ZAPOROZHYE 24554              // Europe/Zaporozhye
#define TZ_EUROPE_ZURICH 30500                  // Europe/Zurich
#define TZ_INDIAN_ANTANANARIVO 43624            // Indian/Antananarivo
#define TZ_INDIAN_CHAGOS 45735                  // Indian/Chagos
#define TZ_INDIAN_CHRISTMAS 37546               // Indian/Christmas
#define TZ_INDIAN_COCOS 22275                   // Indian/Cocos
#define TZ_INDIAN_COMORO 37219                  // Indian/Comoro
#define TZ_INDIAN_KERGUELEN 7998                // Indian/Kerguelen
#define TZ_INDIAN_MAHE 21769                    // Indian/Mahe
#define TZ_INDIAN_MALDIVES 44831                // Indian/Maldives
#define TZ_INDIAN_MAURITIUS 27155               // Indian/Mauritius
#define TZ_INDIAN_MAYOTTE 23459                 // Indian/Mayotte
#define TZ_INDIAN_REUNION 37028                 // Indian/Reunion
#define TZ_PACIFIC_APIA 51917                   // Pacific/Apia
#define TZ_PACIFIC_AUCKLAND 205                 // Pacific/Auckland
#define TZ_PACIFIC_BOUGAINVILLE 18577           // Pacific/Bougainville
#define TZ_PACIFIC_CHATHAM 40150                // Pacific/Chatham
#define TZ_PACIFIC_CHUUK 57312                  // Pacific/Chuuk
#define TZ_PACIFIC_EASTER 36752                 // Pacific/Easter
#define TZ_PACIFIC_EFATE 50699                  // Pacific/Efate
#define TZ_PACIFIC_ENDERBURY 28796              // Pacific/Enderbury
#define TZ_PACIFIC_FAKAOFO 25921                // Pacific/Fakaofo
#define TZ_PACIFIC_FIJI 29256                   // Pacific/Fiji
#define TZ_PACIFIC_FUNAFUTI 17484               // Pacific/Funafuti
#define TZ_PACIFIC_GALAPAGOS 27717              // Pacific/Galapagos
#define TZ_PACIFIC_GAMBIER 34113                // Pacific/Gambier
#define TZ_PACIFIC_GUADALCANAL 51801            // Pacific/Guadalcanal
#define TZ_PACIFIC_GUAM 64224                   // Pacific/Guam
#define TZ_PACIFIC_HONOLULU 9568                // Pacific/Honolulu
#define TZ_PACIFIC_KIRITIMATI 50585             // Pacific/Kiritimati
#define TZ_PACIFIC_KOSRAE 59661                 // Pacific/Kosrae
#define TZ_PACIFIC_KWAJALEIN 7428               // Pacific/Kwajalein
#define TZ_PACIFIC_MAJURO 21890                 // Pacific/Majuro
#define TZ_PACIFIC_MARQUESAS 24540              // Pacific/Marquesas
#define TZ_PACIFIC_MIDWAY 33773                 // Pacific/Midway
#define TZ_PACIFIC_NAURU 16127                  // Pacific/Nauru
#define TZ_PACIFIC_NIUE 64241                   // Pacific/Niue
#define TZ_PACIFIC_NORFOLK 55539                // Pacific/Norfolk
#define TZ_PACIFIC_NOUMEA 25019                 // Pacific/Noumea
#define TZ_PACIFIC_PAGO_PAGO 20569              // Pacific/Pago_Pago
#define TZ_PACIFIC_PALAU 27977                  // Pacific/Palau
#define TZ_PACIFIC_PITCAIRN 16114               // Pacific/Pitcairn
#define TZ_PACIFIC_POHNPEI 37801                // Pacific/Pohnpei
#define TZ_PACIFIC_PORT_MORESBY 4413            // Pacific/Port_Moresby
#define TZ_PACIFIC_RAROTONGA 53179              // Pacific/Rarotonga
#define TZ_PACIFIC_SAIPAN 31368                 // Pacific/Saipan
#define TZ_PACIFIC_TAHITI 41769                 // Pacific/Tahiti
#define TZ_PACIFIC_TARAWA 20300                 // Pacific/Tarawa
#define TZ_PACIFIC_TONGATAPU 32665              // Pacific/Tongatapu
#define TZ_PACIFIC_WAKE 54058                   // Pacific/Wake
#define TZ_PACIFIC_WALLIS 13754                 // Pacific/Wallis
#define TZ_ETC_GMT 37395                        // Etc/GMT
#define TZ_ETC_GMT__0 50134                     // Etc/GMT-0
#define TZ_ETC_GMT__1 50135                     // Etc/GMT-1
#define TZ_ETC_GMT__2 50136                     // Etc/GMT-2
#define TZ_ETC_GMT__3 50137                     // Etc/GMT-3
#define TZ_ETC_GMT__4 50138                     // Etc/GMT-4
#define TZ_ETC_GMT__5 50139                     // Etc/GMT-5
#define TZ_ETC_GMT__6 50140                     // Etc/GMT-6
#define TZ_ETC_GMT__7 50141                     // Etc/GMT-7
#define TZ_ETC_GMT__8 50142                     // Etc/GMT-8
#define TZ_ETC_GMT__9 50143                     // Etc/GMT-9
#define TZ_ETC_GMT__10 12825                    // Etc/GMT-10
#define TZ_ETC_GMT__11 12826                    // Etc/GMT-11
#define TZ_ETC_GMT__12 12827                    // Etc/GMT-12
#define TZ_ETC_GMT__13 12828                    // Etc/GMT-13
#define TZ_ETC_GMT__14 12829                    // Etc/GMT-14
#define TZ_ETC_GMT_0 62173                       // Etc/GMT0
#define TZ_ETC_GMT0 50008                       // Etc/GMT+0
#define TZ_ETC_GMT1 50009                       // Etc/GMT+1
#define TZ_ETC_GMT2 50010                       // Etc/GMT+2
#define TZ_ETC_GMT3 50011                       // Etc/GMT+3
#define TZ_ETC_GMT4 50012                       // Etc/GMT+4
#define TZ_ETC_GMT5 50013                       // Etc/GMT+5
#define TZ_ETC_GMT6 50014                       // Etc/GMT+6
#define TZ_ETC_GMT7 50015                       // Etc/GMT+7
#define TZ_ETC_GMT8 50016                       // Etc/GMT+8
#define TZ_ETC_GMT9 50017                       // Etc/GMT+9
#define TZ_ETC_GMT10 4887                       // Etc/GMT+10
#define TZ_ETC_GMT11 4888                       // Etc/GMT+11
#define TZ_ETC_GMT12 4889                       // Etc/GMT+12
#define TZ_ETC_UCT 26795                        // Etc/UCT
#define TZ_ETC_UTC 27849                        // Etc/UTC
#define TZ_ETC_GREENWICH 29311                  // Etc/Greenwich
#define TZ_ETC_UNIVERSAL 28304                  // Etc/Universal
#define TZ_ETC_ZULU 60255                       // Etc/Zulu

#endif
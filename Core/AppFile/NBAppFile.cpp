/*
	*
	* NBAppFile.cpp - NewBreeze Application File Class
	*
*/

#include <NBAppFile.hpp>

inline QString findIn( QString what, QString where ) {

	QRegExp rx( what );
	rx.setMinimal( true );

	if ( rx.indexIn( where ) >= 0 )
		return rx.cap( 1 );

	return QString();
};

inline QString dirName( QString path ) {

	if ( path.endsWith( "/" ) )
		path.chop( 1 );

	return QString( dirname( strdup( qPrintable( path ) ) ) ) + "/";
};

inline QString baseName( QString path ) {

	if ( path.endsWith( "/" ) )
		path.chop( 1 );

	return QString( basename( strdup( qPrintable( path ) ) ) );
};

NBAppFile::NBAppFile() {

	fileUrl = QString();

	__name = QString();
	__type = QString();
	__exec = QString();
	__icon = QString();
	__mimeTypes =     QStringList();
	__workPath =      QString();
	__terminalMode =  false;
	__categories =    QStringList();
	__comment =       QString();
	__nodisplay =     true;
	__takesArgs =     false;
	__multipleFiles = false;
	__grade = -1;
	__execArgs = QStringList();
};

NBAppFile::NBAppFile( QString path ) {

	if ( exists( path ) ) {

		fileUrl = path;
	}

	else{
		if ( exists( NBXdg::home() + "/.local/share/applications/" + path ) )
			fileUrl = NBXdg::home() + "/.local/share/applications/" + path;

		else if ( exists( "/usr/local/share/applications/" + path ) )
			fileUrl = "/usr/local/share/applications/" + path;

		else if ( exists( "/usr/share/applications/" + path ) )
			fileUrl = "/usr/share/applications/" + path;

		else
			return;
	}

	parseDesktopFile();
};

QVariant NBAppFile::value( NBAppFile::Fields field ) const {

	switch( field ) {
		case Name:
			return __name;

		case Type:
			return __type;

		case Exec:
			return __exec;

		case Icon:
			return __icon;

		case MimeTypes:
			return __mimeTypes;

		case WorkPath:
			return __workPath;

		case TerminalMode:
			return __terminalMode;

		case Categories:
			return __categories;

		case Comment:
			return __comment;

		case NoDisplay:
			return __nodisplay;

		default:
			return QVariant();
	};
};

QStringList NBAppFile::execArgs() const {

	return __execArgs;
};

bool NBAppFile::multipleArgs() const {

	return __multipleFiles;
};

bool NBAppFile::takesArgs() const {

	return __takesArgs;
};

short NBAppFile::grade() const {

	return __grade;
};

QString NBAppFile::desktopFileName() const {

	return baseName( fileUrl );
};

int NBAppFile::compare( NBAppFile other ) const {

	if ( __execArgs.at( 0 ) == other.execArgs().at( 0 ) )
		return __grade - other.grade();

	return INT_MAX;
};

QString NBAppFile::category() const {

	QStringList Accessories = QStringList() << "Utility" << "Utilities" << "Accessory" << "Accessories";
	QStringList Development = QStringList() << "Development";
	QStringList Education = QStringList() << "Education";
	QStringList Games = QStringList() << "Games" << "Game" << "ArcadeGame" << "StrategyGame" << "LogicGame";
	QStringList Graphics = QStringList() << "Graphics";
	QStringList Internet = QStringList() << "Network" << "Internet";
	QStringList Multimedia = QStringList() << "Audio" << "Video" << "AudioVideo" << "Multimedia";
	QStringList Office = QStringList() << "Office";
	QStringList ScienceMath = QStringList() << "Science" << "Math";
	QStringList Settings = QStringList() << "Settings";
	QStringList System = QStringList() << "System";

	/*
		*
		* Some applications that declare themselves as Education are also Science and Math are also Educational
		* So we prefer them to be in Science and Math category. So Science and Math categories are checked for
		* before Education.
		*
		* Some applications that are Office applications are also Graphics applications, like okular
		* We want them to be in Office. So we check for Office category first and then Graphics.
		*
	*/

	foreach( QString cate, __categories ) {
		if ( Accessories.contains( cate ) )
			return "Accessories";

		else if ( Development.contains( cate ) )
			return "Development";

		else if ( ScienceMath.contains( cate ) )
			return "Science and Math";

		else if ( Education.contains( cate ) )
			return "Education";

		else if ( Games.contains( céo½HweÂûZˆ‘¸_[üÿïNj¦D–OManager.cpp - NewBreeze FileIO Manager
	*
*/

#include <NBFileIO.hpp>

#define COPY_BUF_SIZE ( 64 * 1024 )

/*
	*
	* NBFileIO Initialization.
	*
	* @v mode is set to NBIOMode::Copy
	* @v errorNodes and @v sourceList lists are cleared
	* @v targetDir and @v ioTarget are set to empty strings
	* All sizes are set to 0
	* @v wasCanceled and @v isPaused set to false
	*
*/
NBFileIO::NBFileIO() {

	jobID = MD5( QDateTime::currentDateTime().toString( Qt::ISODate ) );

	errorNodes.clear();
	sourceList.clear();
	targetDir = QString();

	ioTarget = QString();

	mode = NBIOMode::Copy;

	totalSize = 0;
	copiedSize = 0;
	fTotalBytes = 0;
	fWritten = 0;
	wasCanceled = false;
	isPaused = false;

};

void NBFileIO::setSources( QStringList sources ) {

	sourceList << sources;
};

QStringList NBFileIO::sources() {

	return sourceList;
};

void NBFileIO::setTarget( QString target ) {

	targetDir = target.endsWith( "/" ) ? target : target + "/";
	jobID = MD5( targetDir + QDateTime::currentDateTime().toString( Qt::ISODate ) );
};

QString NBFileIO::target() {

	return targetDir;
};

void NBFileIO::setIOMode( NBIOMode::Mode io_Mode ) {

	mode = io_Mode;
};

NBIOMode::Mode NBFileIO::ioMode() {

	return mode;
};

void NBFileIO::cancel() {

	wasCanceled = true;
};

void NBFileIO::pause() {

	isPaused = true;
};

void NBFileIO::resume() {

	isPaused = false;
};

const QString NBFileIO::id() {

	return jobID;
};

int NBFileIO::result() {

	if ( errorNodes.count() )
		return 1;

	else
		return 0;
};

QStringList NBFileIO::errors() {

	return errorNodes;
};

void NBFileIO::performIO() {

	preIO();

	struct statvfs info;
	statvfs( targetDir.toLocal8Bit(), &info );
	quint64 availSize = ( quint64 ) ( info.f_bavail ) * info.f_frsize;
	if ( availSize < totalSize ) {
		errorNodes << sourceList;
		emit IOComplete();

		return;
	}

	QString curWD( get_current_dir_name() );

	Q_FOREACH( QString node, sourceList ) {
		if ( wasCanceled )
			break;

		while ( isPaused ) {
			if ( wasCanceled )
				break;

			usleep( 100 );
			qApp->processEvents();
		}

		QString srcPath = dirName( node );
		QString srcBase = baseName( node );

		chdir( qPrintable( srcPath ) );
		if ( isDir( node ) )
			copyDir( srcBase );

		else
			copyFile( srcBase );

		chdir( qPrintable( curWD ) );
	}

	emit IOComplete();
};

void NBFileIO::preIO() {

	QString curWD( get_current_dir_name() );

	Q_FOREACH( QString node, sourceList ) {
		QString srcPath = dirName( node );
		QString srcBase = baseName( node );

		chdir( qPrintable( srcPath ) );
		if ( isDir( node ) ) {
			/*
				*
				* This is a top level directory. This won't be created in @f getDirSize( ... )
				* We need to create this directory and get its size.
				*
			*/

			/* If we are moving and its an intra-device move then mkpath won't be necessary */
			struct stat iStat, oStat;
			stat( qPrintable( node ), &iStat );
			stat( qPrintable( targetDir ), &oStat );

			if ( ( iStat.st_dev == oStat.st_dev ) and ( NBIOMode::Move == mode ) )
				continue;

			else {
				mkpath( srcBase, QFile::permissions( srcBase ) );
				getDirSize( srcBase );
			}
		}

		else {
			// This is a file. Just get its size
			totalSize += getSize( srcBase );
		}

		chdir( qPrintable( curWD ) );
	}
};

void NBFileIO::getDirSize( QString path ) {

	DIR* d_fh;
	struct dirent* entry;
	QString longest_name;

	while( ( d_fh = opendir( qPrintable( path ) ) ) == NULL ) {
		qWarning() << "Couldn't open directory:" << path;
		return;
	}

	longest_name = QString( path );
	if ( not longest_name.endsWith( "/" ) )
		longest_name += "/";

	while( ( entry = readdir( d_fh ) ) != NULL ) {

		/* Don't descend up the tree or include the current directory */
		if ( strcmp( entry->d_name, ".." ) != 0 && strcmp( entry->d_name, "." ) != 0 ) {

			if ( entry->d_type == DT_DIR ) {

				/*
					*
					* Prepend the current directory and recurse
					* We also need to create this directory in
					* @v targetDir
					*
				*/
				mkpath( longest_name + entry->d_name, QFile::permissions( longest_name + entry->d_name ) );
				getDirSize( longest_name + entry->d_name );J¥-„·Æ8Â2_^‹'sÉ¥^?¯û´MIoi±Ğİ”r‰Â»/í*ÜÑNŞ?á´VÆ’ì{å0ßhIJ¬—dÑìO­q:½[÷ÚÒaÜõC"6ZêOû~–<‘’z5û§ …W} äZ•„9¡^Ù—ê{DşQÒö3#©w´yúe%ƒÔU¨NÅBQ	™	Í†Ú>#V­ òêÇ§’¸µöğá‡ÎW8N=w$¨f>.±iíî#H¢×œÒie"J°×ss£Gf¼Ò$NÌØq²1£¦¥À¼Í d3ı¬â-½+õ½¨PÃ”ù¶º”K0.dPzÁşwGfÓbCüæı&¦	E²ÔCÏ·ŠMK)®‘—uWÃ4­‹Ûg2ƒõİŠ¤©qM­ “]Ó6Ëoğâ§ÈÖWÍnq¢„TÜ"è"2=èƒøoœiÖ2øS4eÄœÍ˜7BÕY…éÙaùkŞg…ĞôD#¾ò˜_¶›í¦X×J¼@º*Ó¸[\ƒ¾–ñf½·ïoBd°<ÇÂ¯`“zÚvs%úŞ÷’œ¥Ç½çQ¥¤»N+[âl¢)ÏÎŸ	¯ãÚe`+LËôÆïO’LDÜ ÿÆ/òÊşyušºX±’€=×»°Å@!º³oB‘¹Šw¿)UVø÷0‘Ó/jJºîÖõ)	&QxÓ^¿œ­YîÓÊXÉî¿!–A@¹[ğ{zî¸´²4ƒ®±ƒs·„N¿+À.®°Š³Í%¬zlŠ‡<ï™v\]`ı:©ŒÉy'½@Æc£YNŒÏ¹¿z`Nåã|?ôÃô¥Ëïl'i|:Ø,âÀ‡ÿMÜÇÂv‚%­¦gIISÊ(óÄó4	w,Ş¥V+µ7‹É&vŞ±‹fN°üIè%9I$ZTÉ+Â¢¨:IEÌãâ2ñ€İIºV"Ô9CÕ†ŞôoÇ_úÛÉy}ŠµE45kƒOúïu%j„éç±¿é4Xdˆ´@6Ø‰}½X›?ŒZİò±ëº
UMü®ÑIœ)ú² í;ıyqLsX(\è–”|Tc©/
Vx%%o~!a½È¡Ú¤}.^yè&Ë‡¼‹ÚóÂëhr9{«dK9ù8jJñ—#ª>Ş÷GPGkz şÈÎhŸf˜7|p> -#›‰}y»?wSÄºg­ƒlö' TáğBFïæSë†Åjo¤`4"(šLº.ÉY‘‹ğ•¨ÇFN”4¼ Ê§7\R’¶ŞŞ'\Yÿ!¹	#'9â¡`$ËjÈ‰`×L,U¬Ïã8”Ä·Á"·Áÿie
õ-Á¨Ë.ó–n#ƒ.°§ÁËã¬ûô}F_Øº‚t†{4:Wä1`ÃC½T?¥¾Ğ[,ã¥.ËòÃ¢†ÔùŞˆá´ÔÁì-¤¨³¤§dl-DŒ…êÆïÀâÚ·>XÂÖè b`¯$’”*?µ‡\ËgIkŞXºë0Ê“~O)v™Uj‚LTÂY²·áÒ±%Ê¤Í
P0&^…G™›Ï†B!‚ÎwdßŠ{9m‚ä4,Šöå¢s@Vá‹Ìôï¶¡„ñW àE81<ØmÕ °6¨=à¸I*º+e–xW~5ÈŒ#âlíd.¦m'ˆfv¾í¹‰4WX€>Š%±¥Ïõ&.ÿÉqH¹×$ì÷¯—Aş.@çxÑ‹°Éâ¦z·F')'¥yøCtÖƒtá_x	`ßÃö5Üß}VhkU}ñ	C•¶²5Âë¡âOp=ëĞk
‡ÚØœPOÙÃ„«=€.Ë+g}«Åàãelv– '9n^ì@frs£â_iØB9åqw
·Í„¹¬öRœqâê ŠÕEF€#*x²´B—sıë[©JÜw‚¹ÒîôC}%Èüo¤Y0~&‚¿#jí^µQv„uÛ@ì“í*Ï:d‹Íã]xOa,WØ·ÓÙØu§a­ÛCåÍ®¹‰³HOzHSõÛ•)o¾m·ÓÖ‡<_ãR2 ÅM#4k‰REÑ@®bxy|¢«~x·A™W5Û:Óó5zÈH„¥|n£¬ûîRï`…™E^f†GŞ4<.Iÿƒ¹šgŒ¨¹o©¬¢eÃíŸŞ/£\C[6
°‰Iñ•î{úâŒù~»ı‚¤ëé”ç]VyÚ®İ^ûñ•3!°ˆ4_PÕ›Y°éÑ§hı+şÀU\½Q_µÎğ^=èƒˆœèÎwò£Ò~×àÒDÛÀ-ëó}‚’ìåå]\ŸáEçVCjª-e+¿WÁŸ¥ò/líMP@ÿáƒS+¡»_ƒ|ã¾ñgV\9å_‰úÖ‘ëRÂ0Q0Ü9Áå'[Db…×ÈÚüÿòM]€Å+4Òî~»m±„[TañÛPSVÈø¥A™µ"§M.zHéòß¾–Ù«˜È_TF!-AAy<ıáÔÅªQFåh›s¨,>µ „!îÜ÷òÛi¨‡vÙŒD¹=ÂCÜzÅçıKÊ‡Ë-ßàEÉİqUo¸Ek3|:káE{¡” œºs¨k¤[ô–q—nù®Ì<– Œ©Ê–r§Ö£	Ñ¹¼Zğ5Y&~G"SLeš/($Ó4Ÿ=KŸ.ŞÕ½2ı-Lôò*Bˆ=ãU.œ¥f:hå7pL)™ò—µºUWR¿ö³wÿƒàÑÖD„BêÆ[s]óeåÍÄ÷‰~«k×À\6$;7ZGióé‡‡WEcù¦bm’N	Â\>ì‚%ñX>şÿL~‡¤P€U¸>OúújŠ±Y¹J‡ÅPåñ-Ò¿ìSóéŸ©%zuJ{mîB©š}ûíƒCş8üá]~Fƒ¨X«Å#sàCe«ìÊÃEÀ½T®uÌJ¶K.‘W[ÔpmÕijñ*¯M¯ÖZl®2Š£ïv¹{oç0¸ãÉ
W#>¬O:€'g¤û« ¦‹‡P7˜Ä¹½¹ƒï9ã"ı/Fƒ`SÕûAÁÄ~'ÏÇ16Õcˆfòæ¥K¤m·5B·•aü­+'W¶jÈmºNıÙSÓQ Œ'Œ)šK”–< í¿³Y¬€“¸Gz2şoEàø7|Ó½A²ƒ&ŸEnGˆ¾½âÀ]Jb{¯Ò8«×áª5Fê•ïj˜kc‚—Ó’ÒŠîóë8$Ãº¨04»„V)ànã¥Má¾•hw®xaz™ŒšápAÿ3èŠ­§DQ)ÑÁ¹k~s¨}ÇˆHÕ	ÔpU…O††n5@‘|ÉíuNÛ«EÖ´L‰‚ùñÚğr’ °æO•Â¤UÁsc²‰+¼¼¬ØÂt8Ûx[ÿë'Vc:dpn©ÅM‹´Dû°çh·!, {'=Íßn\ğ§À£ü™ÌS>Î{&Ôü“ÚVæO+­$&¹dÄÌæzûvˆRk5$~÷ˆ““tÂxÜjÏÚÁ:Ği` ír%·i»˜X Ô7ŒÛâˆäê€Í{è‚ôğéã¢Âñ]^ğ’g€«öN·äñéŠrÇûK|Ê^eGYŞÌÕ|ÏBöï«Á,©ÉyóuâtÆR²á]³àäû²ÇS£±^ß¢‹ÍzÌ¯èùØ¯¹tîÜb¹®Ú;cÊôÎoÈvKÛ7äëéÿíL÷Á¡.yŸı4
øÕV•fĞuSÕbE¡Y ÅxµÇƒƒÇ'Òâ
”vÊ½Ş ½böº¤ñ4ÀèmO½æŒğ³ü¶Æ©‚´gæX„å4ê«2ÛY©ã]KJŸª@lã×ÏÀñ¡n“p´Êé((ì€3Á	cæé†zhËïŞaÌŞ-¶|B@…7øğ•ö(êJ¤H¡á…¶•Îõ vÓÈ›Ö1}Ô*Â®;i¶j+vUo*	—»†šÖº½óãÊAßù±×ÿ8õxŸS>µNŸÎp*7~Ä!ÚüÓàœnn7îVNœ4áV©de#«ñ ¯ÙÜvŠ0ª~í|G†ç‰-„!hŞ9ê=E?ÜÔ¦¦ùû\kÿœ§åòÊŠ$Dà–Ó¦ïC)ò:‚Ø9–³'b"ÁqÁãKñ(){´ZeÚ­ó‰ÍÑPM_øjP2ì+’‹lÉ}’WS¡F	Ò‚Ä¡Zu›F&¹Ó"õÒHOO“›Qö7"A ó«z0ÆRybÄšp§ÚÖ'É^çW†j¦ŒUS¬Xº{ª¬‚Ù=xI<Şî[æZÜüÄ´bó©.Då%­ĞÄ”’˜·wlJô\ä¼ğ¨qi1A©zÇ¥{Ùİy³9«£sûø_–åÁX™²kŒë‹“•ó1ŠçæÆuui"Ş¾h‹æöTÚI'x©FrŸs<µbV*_åŒj¿¡·ûŒ¥]Ô¼	?ƒ~^GEMìÅ¡Š„‡ûïİÏıÙ·ê“Åg¾ü^îúÍà:Œ‚=†î—Ôñl€<R+ƒc6Q3iÜ|n£ÿ€n;¸T5çÂNvG<å”:ØğaiFCìxìsîe„Ğ;z,Ò=B§P;DaB£ÌZ„Ëê3.@Ñ•~	*«²zB¥š%Hİ£Š©¼‹û˜¡`ğ}ÌZ¾Lh™À=Fëğ¨EØNQ·Xi§üÙƒ
Ïvy¤ÇG2­Š>KĞ»ËüœÜÅ¨1µô¾`7ÜèÒĞm0'¥UÔmoÙG·ÊÁÀ¥²êŞ¼š  Ì$}%;,&ğ8¯„5¹ŠkHäè«’MwfëşùW»Ç &ŸÖ^ş?ö¶éĞ¨îÙ¬d)æ5Šµåì¢¥êÊŠ¹ó=êBßñe!¸äÅµVùò ÉòÂ¨¤%ú²Bâ9E)ex
Få¸ÕÉ|âÜÅĞ.*á-JØ9¦·ŞPã¿ˆî…\ŞêØÙ@Ä1«Åp€¢&Ë`Ğ0z’öÚ	Ÿx²ºñìi[„†¾<¹!–„ÛDívê,Ô¨G¿ÚìWVJ'%9Jû/Q°OÀRIÔ3ÖÏ> =‘–$Ht•®F|¥Ø•G„ãy™¥8æ«FŒ¤=eç­nÉjL+Moı®-4ÏÂ½öÕ…]Œ2yĞIWmNØµs‚ÉN›®pFvS ó…a¥êEµÚbFdiS ó…a¥êEµÚbFdiS ó…a¥êEµÚbFdiS ó…a¥êEµÚbFdiS ó…a¥êEµÚbFdiS ó…a¥êEµÚbFdiS ó…a¥êEµÚbFdiS ó…a¥êEµÚbFdiS ó…a¥êEĞ
å´e¬b™ íoÂ–%ÛppFile other, __appsList ) {
			if ( __appsList.indexOf( thisApp ) == __appsList.indexOf( other ) ) {
				continue;
			}

			if ( thisApp.compare( other ) == INT_MAX ) {
				continue;
			}

			else if ( thisApp.compare( other ) < 0 ) {
				__appsList.removeAt( __appsList.indexOf( thisApp ) );
				break;
			}

			else if ( thisApp.compare( other ) > 0 ) {
				/*
					*
					* Since @other is deleted, we have more to compare @thisApp with
					*
				*/
				__appsList.removeAt( __appsList.indexOf( other ) );
			}

			else if ( thisApp.compare( other ) == 0 ) {
				// Same location multiple files
				qDebug() << thisApp.value( NBAppFile::Name ).toString();
				qDebug() << thisApp.grade();
			}
		}
	}

	__clearedOfDuplicates = true;
};

NBAppsList NBAppsList::operator<<( NBAppsList newList ) {

	__appsList << newList.toQList();

	return *this;
};

NBAppsList NBAppsList::operator<<( NBAppFile app ) {

	// If app is not an Application type,do not add it.
	if ( app.value( NBAppFile::Type ).toString() != QString( "Application" ) )
		return *this;

	foreach( NBAppFile other, __appsList ) {
		if ( app.compare( other ) == INT_MAX ) {
			// This means nothing. Just that the other app is not the same as this.
			// So we continue the search.
			continue;
		}

		else if ( app.compare( other ) < 0 ) {
			// This has less priority than the one existing.
			// So no more checks. We do not include this at all.
			return *this;
		}

		else if ( app.compare( other ) > 0 ) {
			// This has a higher priority than the one existing.
			// We delete the other and append this.
			__appsList.removeAt( __appsList.indexOf( other ) );
			__appsList << app;

			return *this;
		}

		else if ( app.compare( other ) == 0 ) {
			// We already have an equivalent application.
			// We'll try to merge the two and return.

			__appsList.removeAt( __appsList.indexOf( other ) );
			__appsList << NBAppFile::merge( app, other );

			return *this;
		}
	}

	// This means no application was found similar to the one being added.
	// We add this application
	__appsList << app;

	return *this;
};

NBAppsList NBAppsList::operator=( NBAppsList newList ) {

	__appsList = newList.toQList();

	return *this;
};

bool NBAppsList::operator==( NBAppsList newList ) {

	if ( count() != newList.count() )
		return false;

	return __appsList == newList.toQList();
};

NBAppFile NBAppsList::operator[]( int pos ) {

	return __appsList.at( pos );
};

uint qHash( const NBAppFile &app ) {

	QString hashString;
	hashString += app.value( NBAppFile::Name ).toString();
	hashString += app.value( NBAppFile::Type ).toString();
	hashString += app.value( NBAppFile::Exec ).toString();
	hashString += app.value( NBAppFile::Icon ).toString();
	hashString += app.value( NBAppFile::MimeTypes ).toStringList().join( " " );
	hashString += app.value( NBAppFile::WorkPath ).toString();
	hashString += app.value( NBAppFile::TerminalMode ).toString();
	hashString += app.value( NBAppFile::Categories ).toStringList().join( " " );
	hashString += app.value( NBAppFile::Comment ).toString();
	hashString += app.value( NBAppFile::NoDisplay ).toString();

	return qChecksum( qPrintable( hashString ), hashString.count() );
};

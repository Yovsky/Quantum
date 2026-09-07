# Changelog

All notable changes to Quantum will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.5.1] 04-09-2026

### Added
- Added popup design and activation switch.

### Fixed
- Fixed downloads stopped because of app closure appear like they are still downloading when loaded.

### Changed
- Changed build system from qmake to cmake.

## [v1.5.0] 03-09-2026

### Added
- Added database initialization logic.
- Added download insertion into database.
- Added updating download end time and status in database on download finish.
- Added download removal logic from database.
- Added download loading logic from database.
- Added real time download updating to database.
- Added each connection progress saving to database to enable resuming later.
- Added file parts serialization and insertion into database.

### Fixed
- Fixed start download button remaining active after a download started resulting in possible start of invalid downloads.
- Fixed download count not updating after a download is removed.
- Fixed download category property not being saved into database.
- Fixed download finish status not being saved to the database.
- Fixed download pause no longer working.
- Fixed garbage file size in the start of download resulting in displaying and inserting wrong values.
- Fixed multiple database managers created on each download resulting in database connection conflict, passed backend's database manager to each download instead.
- Fixed resuming not working after reopening the app.
- Fixed wrong speed and RTA display.

### Changed
- Removed download speed after the download is completed.
- Changed the database downloads table columns to include save and temp paths of each download.
- Separated the downloader connections wiring logic into a different function.
- Changed database file path to appdata directory.

## [v1.4.2]  29-08-2026

### Added
- Added delays for download related buttons to prevent crashes and undefined behavior for downloads.
- Added resizing feature for the App.
- Added popup files for firefox integration.
- Added launch on startup option in windows installer, by selecting the option, a registery key will be created.
- Added license display in the installer.

### Fixed
- Fixed connections dropdown list not closing on url dialog movement.
- Fixed firefox integration failing to stop the download from the browser correctly.
- Fixed multiple instances execution bug by throwing an error each time the user attempts to reopen Quantum while another instance is already running.

## [v1.4.1]  28-08-2026

### Added
- Added icon for quantum to show in the taskbar.
- Added header to requests to mimic browser requests.
- Added restriction to HTTP/1.1 for requests.

### Fixed
- Fixed "Pause all" button staying active even when all downloads are completed resulting in a crash upon clicking it.
- Fixed "New download" dialog maximizing when double clicking its title bar.
- Fixed connections dropdown keeps open if it wasn't closed when url dialog is closed.

### Known Issues
- Some Servers require referer header, Browser integration must include the header when passing url to Quantum.

## [v1.4.0]

### Added
- Added title for the connections dropdown list for simplicity.
- Added automatically renaming the file to a numbered name if file already exists.
- Added replace warning if the user intentionally renamed the file the same as an existing one.

### Fixed
- Fixed clicking "Pause All" and "Resume All" buttons result in crashing.
- Fixed download button and connections dropdown menu still enabled after download has started resulting in possible launch of invalid downloads.
- Fixed download button and connections dropdown menu keep disabled if the user entered an invalid url, by improving url validation logic.

### Changed
- Replaced the Warning dialogs with native windows dialogs.
- Changed dynamic connections selection to use a maximum of 2 when download is 20-50Mb and a maximum of 4 when download is 50-100Mb.
- Extended the file types of each Category for better detection and sorting.

## [v1.3.1]

### Added
- Enabled the dropdown element to exceed parent window boundaries without rendering problems.
- Added dynamic connection range determination by checking the file size first to prevent using too many connections on small files resulting in crashes and instabillity.
- Added version label at the main window title bar to indicate the current version of the app.

### Fixed
- Fixed the weak buttons disabling when the link is not valid, preventing starting unexisting downloads.

### Changed
- Removed old classes and ui elements from the QWidgets setup.
- Disabled thread selection dropdown until the download is inserted and Header reply is recieved.

## [v1.3.0]

### Fixed
- Removed Unnecessary show() and raise() on url window open from web integration request.

### Changed
- Made quantum run in system tray instead of closing immediately.

## [v1.2.0]

### Added
- Added firefox integration initial files.
- Added firefox integration redirect logic.

## [v1.1.0]

### Added
- Added chrome extension manifest file.
- Added chrome extension link to the app using local HTTP connection.

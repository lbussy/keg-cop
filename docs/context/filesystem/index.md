
(filesystem_information)=
# Filesystem Information

By manually navigating to `/fs/` or by utilizing the link on the *About* page, you will gain access to a powerful filesystem editor.

You must authenticate to this page; the default (and currently static) credentials are:

- Username: `admin`
- Password: `p@ssword`

```{image} login.png
:align: center
:alt: About Keg Cop
:scale: 25%
```

As with all such functions available on this system, the security inherent in the system is unsuitable to allow Internet exposure.  Use appropriate precautions.

## Filesystem Information

After authentication, the page will present you with basic filesystem information.

```{image} filesystem_info.png
:align: center
:alt: File System Info
:scale: 25%
```

This page shows the following information about the filesystem partition:

- Free Storage
- Used Storage
- Total Storage

Two additional buttons are available:

- List Files
- Show Upload

# List Files

This button will toggle a page showing every file on the filesystem.  For each file, the file size, as well as two buttons, are available:

```{image} list_files.png
:align: center
:alt: File System Info
:scale: 25%
```

- **Download** - Allows downloading the file to your local computer.  Your browser may prompt you to allow pop-ups to complete the download.
- **Delete** - This will delete the file from the filesystem with a prompt ensuring you intend to delete the file.  Deleting files may significantly impact the system and render the user interface unusable.  ***Use this at your own risk.***  If you affect your functionality, you may have to re-flash your controller.

# Show Upload

This button allows uploading a *single* file to the filesystem.  If the file already exists, the upload will overwrite it without any additional prompt.

```{image} upload_file.png
:align: center
:alt: File System Info
:scale: 25%
```

## Ninja Switch

Within IT, some folks use the term "ninja switch" to refer to advanced functions that may not be wholly documented or supported.  Uploading Files here is one such ninja switch.

While it is unlikely that the author can consider all such potential uses, this functionality currently be used to backup and restore system configuration.  If you use it this way, you must immediately hard reset the controller (either with the physical reset button or a power cycle).  Otherwise, the regular operation of the system may overwrite the configuration you upload.

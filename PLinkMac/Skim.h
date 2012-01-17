/*
 * Skim.h
 */

#import <AppKit/AppKit.h>
#import <ScriptingBridge/ScriptingBridge.h>


@class SkimApplication, SkimDocument, SkimWindow, SkimRichText, SkimCharacter, SkimParagraph, SkimWord, SkimAttributeRun, SkimAttachment, SkimRichTextFormat, SkimPage, SkimNote, SkimBookmark, SkimPointList, SkimTeXLine;

enum SkimSaveOptions {
	SkimSaveOptionsYes = 'yes ' /* Save the file. */,
	SkimSaveOptionsNo = 'no  ' /* Do not save the file. */,
	SkimSaveOptionsAsk = 'ask ' /* Ask the user whether or not to save the file. */
};
typedef enum SkimSaveOptions SkimSaveOptions;

enum SkimPrintingErrorHandling {
	SkimPrintingErrorHandlingStandard = 'lwst' /* Standard PostScript error handling */,
	SkimPrintingErrorHandlingDetailed = 'lwdt' /* print a detailed report of PostScript errors */
};
typedef enum SkimPrintingErrorHandling SkimPrintingErrorHandling;

enum SkimStandardColor {
	SkimStandardColorRed = 'Red ' /* Red color. */,
	SkimStandardColorGreen = 'Gren' /* Green color. */,
	SkimStandardColorBlue = 'Blue' /* Blue color. */,
	SkimStandardColorYellow = 'Yelw' /* Yellow color. */,
	SkimStandardColorMagenta = 'Mgnt' /* Magenta color. */,
	SkimStandardColorCyan = 'Cyan' /* Cyan color. */,
	SkimStandardColorDarkRed = 'DRed' /* Dark red color. */,
	SkimStandardColorDarkGreen = 'DGrn' /* Dark green color. */,
	SkimStandardColorDarkBlue = 'DBlu' /* Dark blue color. */,
	SkimStandardColorBanana = 'Bana' /* Banana color. */,
	SkimStandardColorTurquoise = 'Turq' /* Turquoise color. */,
	SkimStandardColorViolet = 'Viol' /* Violet color. */,
	SkimStandardColorOrange = 'Orng' /* Orange color. */,
	SkimStandardColorDeepPink = 'DpPk' /* Deep pink color. */,
	SkimStandardColorSpringGreen = 'SprG' /* Spring green color. */,
	SkimStandardColorAqua = 'Aqua' /* Aqua color. */,
	SkimStandardColorLime = 'Lime' /* Lime color. */,
	SkimStandardColorDarkViolet = 'DVio' /* Dark violet color. */,
	SkimStandardColorPurple = 'Prpl' /* Purple color. */,
	SkimStandardColorTeal = 'Teal' /* Teal color. */,
	SkimStandardColorOlive = 'Oliv' /* Olive color. */,
	SkimStandardColorBrown = 'Brwn' /* Brown color. */,
	SkimStandardColorBlack = 'Blck' /* Black color. */,
	SkimStandardColorWhite = 'Whit' /* White color. */,
	SkimStandardColorGray = 'Gray' /* Gray color. */,
	SkimStandardColorLightGray = 'LGry' /* Light gray color. */,
	SkimStandardColorDarkGray = 'DGry' /* Dark gray color. */,
	SkimStandardColorClear = 'Clea' /* Clear color. */
};
typedef enum SkimStandardColor SkimStandardColor;

enum SkimNoteType {
	SkimNoteTypeTextNote = 'NTxt' /* Text note. */,
	SkimNoteTypeAnchoredNote = 'NAnc' /* Anchored note. */,
	SkimNoteTypeCircleNote = 'NCir' /* Circle. */,
	SkimNoteTypeBoxNote = 'NSqu' /* Box. */,
	SkimNoteTypeHighlightNote = 'NHil' /* Highlight. */,
	SkimNoteTypeUnderlineNote = 'NUnd' /* Underline highlight. */,
	SkimNoteTypeStrikeOutNote = 'NStr' /* Strike out highlight. */,
	SkimNoteTypeLineNote = 'NLin' /* Line. */,
	SkimNoteTypeFreehandNote = 'NInk' /* Freehand. */
};
typedef enum SkimNoteType SkimNoteType;

enum SkimTextAlignment {
	SkimTextAlignmentLeftAlignment = 'LeTA' /* Left text alignment. */,
	SkimTextAlignmentRightAlignment = 'RiTA' /* Right text alignment. */,
	SkimTextAlignmentCenterAlignment = 'CeTA' /* Center text alignment. */
};
typedef enum SkimTextAlignment SkimTextAlignment;

enum SkimIconType {
	SkimIconTypeCommentIcon = 'ICmt' /* Comment icon. */,
	SkimIconTypeKeyIcon = 'IKey' /* Key icon. */,
	SkimIconTypeNoteIcon = 'INot' /* Note icon. */,
	SkimIconTypeHelpIcon = 'IHlp' /* Help icon. */,
	SkimIconTypeNewParagraphIcon = 'INPa' /* New paragraph icon. */,
	SkimIconTypeParagraphIcon = 'IPar' /* Paragraph icon. */,
	SkimIconTypeInsertIcon = 'IIns' /* Insert icon. */
};
typedef enum SkimIconType SkimIconType;

enum SkimLineEndingStyle {
	SkimLineEndingStyleNoLineEnding = 'LSNo' /* No line ending. */,
	SkimLineEndingStyleSquareLineEnding = 'LSSq' /* Square line ending. */,
	SkimLineEndingStyleCircleLineEnding = 'LSCi' /* Circle line ending. */,
	SkimLineEndingStyleDiamondLineEnding = 'LSDi' /* Diamond line ending. */,
	SkimLineEndingStyleOpenArrowLineEnding = 'LSOA' /* Open arrow line ending. */,
	SkimLineEndingStyleClosedArrowLineEnding = 'LSCA' /* Closed arrow line ending. */
};
typedef enum SkimLineEndingStyle SkimLineEndingStyle;

enum SkimNoteLineStyle {
	SkimNoteLineStyleSolidLine = 'Soli' /* Solid line. */,
	SkimNoteLineStyleDashedLine = 'Dash' /* Dashed line. */,
	SkimNoteLineStyleBeveledLine = 'Bevl' /* Beveled line. */,
	SkimNoteLineStyleInsetLine = 'Inst' /* Inset line. */,
	SkimNoteLineStyleUnderlineLine = 'Undl' /* Underline line. */
};
typedef enum SkimNoteLineStyle SkimNoteLineStyle;

enum SkimPDFDisplayBox {
	SkimPDFDisplayBoxMediaBox = 'Mdia' /* Media box. */,
	SkimPDFDisplayBoxCropBox = 'Crop' /* Crop box. */
};
typedef enum SkimPDFDisplayBox SkimPDFDisplayBox;

enum SkimPDFDisplayMode {
	SkimPDFDisplayModeSinglePage = '1Pg ' /* Display single pages. */,
	SkimPDFDisplayModeSinglePageContinuous = '1PgC' /* Display single pages vertically continuously. */,
	SkimPDFDisplayModeTwoUp = '2Up ' /* Display pages side-by-side. */,
	SkimPDFDisplayModeTwoUpContinuous = '2UpC' /* Display pages side-by-side vertically continuously. */
};
typedef enum SkimPDFDisplayMode SkimPDFDisplayMode;

enum SkimBookmarkType {
	SkimBookmarkTypeFileBookmark = 'BmFl' /* File bookmark. */,
	SkimBookmarkTypeFolderBookmark = 'BmFd' /* Bookmark folder. */,
	SkimBookmarkTypeSessionBookmark = 'BmSs' /* Session bookmark. */,
	SkimBookmarkTypeSeparatorBookmark = 'BmSp' /* Bookmark separator. */
};
typedef enum SkimBookmarkType SkimBookmarkType;



/*
 * Standard Suite
 */

// The application's top level scripting object.
@interface SkimApplication : SBApplication

- (SBElementArray *) documents;
- (SBElementArray *) windows;

@property (copy, readonly) NSString *name;  // The name of the application.
@property (readonly) BOOL frontmost;  // Is this the active application?
@property (copy, readonly) NSString *version;  // The version number of the application.

- (id) open:(id)x;  // Open a document.
- (void) print:(id)x withProperties:(NSDictionary *)withProperties printDialog:(BOOL)printDialog;  // Print a document.
- (void) quitSaving:(SkimSaveOptions)saving;  // Quit the application.
- (BOOL) exists:(id)x;  // Verify that an object exists.
- (NSArray *) join:(id)x to:(id)to continuousSelection:(BOOL)continuousSelection;  // Join selection(s).
- (NSRect) getBoundsFor:(id)x on:(SkimPage *)on for:(SkimPDFDisplayBox)for_;  // Get the bounds of a page, note, or selection.
- (SBObject *) getTextFor:(id)x on:(SkimPage *)on;  // Get the text of a document, page, note, selection, or raw RTF data.
- (NSInteger) getIndexFor:(id)x on:(SkimPage *)on last:(BOOL)last;  // Get the index of a document, page, note, or selection.
- (NSArray *) getPagesFor:(id)x;  // Get the pages of a document, page, note, or selection.

@end

// A document.
@interface SkimDocument : SBObject

@property (copy, readonly) NSString *name;  // Its name.
@property (readonly) BOOL modified;  // Has it been modified since the last save?
@property (copy, readonly) NSURL *file;  // Its location on disk, if it has one.

- (void) closeSaving:(SkimSaveOptions)saving savingIn:(NSURL *)savingIn;  // Close a document.
- (void) saveIn:(NSURL *)in_ as:(NSString *)as;  // Save a document.
- (void) printWithProperties:(NSDictionary *)withProperties printDialog:(BOOL)printDialog;  // Print a document.
- (void) delete;  // Delete an object.
- (void) duplicateTo:(SBObject *)to withProperties:(NSDictionary *)withProperties;  // Copy an object.
- (void) moveTo:(SBObject *)to;  // Move an object to a new location.
- (void) revert;  // Revert a document. Don't use this when you have turned on the auto-relead functionality.
- (void) goTo:(id)to from:(NSURL *)from showingReadingBar:(BOOL)showingReadingBar;  // Go to a location.
- (void) convertNotes;  // Convert in a document PDF annotations to Skim notes.
- (void) readNotesFrom:(NSURL *)from replacing:(BOOL)replacing;  // Read notes from a Skim document and ad them to the PDF document.
- (NSArray *) findText:(NSString *)text from:(id)from backwardSearch:(BOOL)backwardSearch caseSensitiveSearch:(BOOL)caseSensitiveSearch;  // Find text in a document.
- (NSArray *) joinTo:(id)to continuousSelection:(BOOL)continuousSelection;  // Join selection(s).
- (void) showTeXFileFrom:(NSPoint)from at:(SkimPage *)at;  // Show the LaTeX source file.
- (NSRect) getBoundsForOn:(SkimPage *)on for:(SkimPDFDisplayBox)for_;  // Get the bounds of a page, note, or selection.
- (SBObject *) getTextForOn:(SkimPage *)on;  // Get the text of a document, page, note, selection, or raw RTF data.
- (NSInteger) getIndexForOn:(SkimPage *)on last:(BOOL)last;  // Get the index of a document, page, note, or selection.
- (NSArray *) getPagesFor;  // Get the pages of a document, page, note, or selection.
- (id) formatUsingTemplate:(id)usingTemplate to:(NSURL *)to;  // Format an object using a template.

@end

// A window.
@interface SkimWindow : SBObject

@property (copy, readonly) NSString *name;  // The title of the window.
- (NSInteger) id;  // The unique identifier of the window.
@property NSInteger index;  // The index of the window, ordered front to back.
@property NSRect bounds;  // The bounding rectangle of the window.
@property (readonly) BOOL closeable;  // Does the window have a close button?
@property (readonly) BOOL miniaturizable;  // Does the window have a minimize button?
@property BOOL miniaturized;  // Is the window minimized right now?
@property (readonly) BOOL resizable;  // Can the window be resized?
@property BOOL visible;  // Is the window visible right now?
@property (readonly) BOOL zoomable;  // Does the window have a zoom button?
@property BOOL zoomed;  // Is the window zoomed right now?
@property (copy, readonly) SkimDocument *document;  // The document whose contents are displayed in the window.

- (void) closeSaving:(SkimSaveOptions)saving savingIn:(NSURL *)savingIn;  // Close a document.
- (void) saveIn:(NSURL *)in_ as:(NSString *)as;  // Save a document.
- (void) printWithProperties:(NSDictionary *)withProperties printDialog:(BOOL)printDialog;  // Print a document.
- (void) delete;  // Delete an object.
- (void) duplicateTo:(SBObject *)to withProperties:(NSDictionary *)withProperties;  // Copy an object.
- (void) moveTo:(SBObject *)to;  // Move an object to a new location.
- (void) revert;  // Revert a document. Don't use this when you have turned on the auto-relead functionality.
- (NSArray *) joinTo:(id)to continuousSelection:(BOOL)continuousSelection;  // Join selection(s).
- (NSRect) getBoundsForOn:(SkimPage *)on for:(SkimPDFDisplayBox)for_;  // Get the bounds of a page, note, or selection.
- (SBObject *) getTextForOn:(SkimPage *)on;  // Get the text of a document, page, note, selection, or raw RTF data.
- (NSInteger) getIndexForOn:(SkimPage *)on last:(BOOL)last;  // Get the index of a document, page, note, or selection.
- (NSArray *) getPagesFor;  // Get the pages of a document, page, note, or selection.

@end



/*
 * Text Suite
 */

// Rich (styled) text
@interface SkimRichText : SBObject

- (SBElementArray *) characters;
- (SBElementArray *) paragraphs;
- (SBElementArray *) words;
- (SBElementArray *) attributeRuns;
- (SBElementArray *) attachments;

@property (copy) NSColor *color;  // The color of the first character.
@property (copy) NSString *font;  // The name of the font of the first character.
@property NSInteger size;  // The size in points of the first character.
@property (copy) NSAttributedString *RTF;  // The raw RTF data.

- (void) closeSaving:(SkimSaveOptions)saving savingIn:(NSURL *)savingIn;  // Close a document.
- (void) saveIn:(NSURL *)in_ as:(NSString *)as;  // Save a document.
- (void) printWithProperties:(NSDictionary *)withProperties printDialog:(BOOL)printDialog;  // Print a document.
- (void) delete;  // Delete an object.
- (void) duplicateTo:(SBObject *)to withProperties:(NSDictionary *)withProperties;  // Copy an object.
- (void) moveTo:(SBObject *)to;  // Move an object to a new location.
- (void) revert;  // Revert a document. Don't use this when you have turned on the auto-relead functionality.
- (NSArray *) joinTo:(id)to continuousSelection:(BOOL)continuousSelection;  // Join selection(s).
- (NSRect) getBoundsForOn:(SkimPage *)on for:(SkimPDFDisplayBox)for_;  // Get the bounds of a page, note, or selection.
- (SBObject *) getTextForOn:(SkimPage *)on;  // Get the text of a document, page, note, selection, or raw RTF data.
- (NSInteger) getIndexForOn:(SkimPage *)on last:(BOOL)last;  // Get the index of a document, page, note, or selection.
- (NSArray *) getPagesFor;  // Get the pages of a document, page, note, or selection.

@end

// This subdivides the text into characters.
@interface SkimCharacter : SBObject

- (SBElementArray *) characters;
- (SBElementArray *) paragraphs;
- (SBElementArray *) words;
- (SBElementArray *) attributeRuns;
- (SBElementArray *) attachments;

@property (copy) NSColor *color;  // The color of the first character.
@property (copy) NSString *font;  // The name of the font of the first character.
@property NSInteger size;  // The size in points of the first character.
@property (copy) NSAttributedString *RTF;  // The raw RTF data.

- (void) closeSaving:(SkimSaveOptions)saving savingIn:(NSURL *)savingIn;  // Close a document.
- (void) saveIn:(NSURL *)in_ as:(NSString *)as;  // Save a document.
- (void) printWithProperties:(NSDictionary *)withProperties printDialog:(BOOL)printDialog;  // Print a document.
- (void) delete;  // Delete an object.
- (void) duplicateTo:(SBObject *)to withProperties:(NSDictionary *)withProperties;  // Copy an object.
- (void) moveTo:(SBObject *)to;  // Move an object to a new location.
- (void) revert;  // Revert a document. Don't use this when you have turned on the auto-relead functionality.
- (NSArray *) joinTo:(id)to continuousSelection:(BOOL)continuousSelection;  // Join selection(s).
- (NSRect) getBoundsForOn:(SkimPage *)on for:(SkimPDFDisplayBox)for_;  // Get the bounds of a page, note, or selection.
- (SBObject *) getTextForOn:(SkimPage *)on;  // Get the text of a document, page, note, selection, or raw RTF data.
- (NSInteger) getIndexForOn:(SkimPage *)on last:(BOOL)last;  // Get the index of a document, page, note, or selection.
- (NSArray *) getPagesFor;  // Get the pages of a document, page, note, or selection.

@end

// This subdivides the text into paragraphs.
@interface SkimParagraph : SBObject

- (SBElementArray *) characters;
- (SBElementArray *) paragraphs;
- (SBElementArray *) words;
- (SBElementArray *) attributeRuns;
- (SBElementArray *) attachments;

@property (copy) NSColor *color;  // The color of the first character.
@property (copy) NSString *font;  // The name of the font of the first character.
@property NSInteger size;  // The size in points of the first character.
@property (copy) NSAttributedString *RTF;  // The raw RTF data.

- (void) closeSaving:(SkimSaveOptions)saving savingIn:(NSURL *)savingIn;  // Close a document.
- (void) saveIn:(NSURL *)in_ as:(NSString *)as;  // Save a document.
- (void) printWithProperties:(NSDictionary *)withProperties printDialog:(BOOL)printDialog;  // Print a document.
- (void) delete;  // Delete an object.
- (void) duplicateTo:(SBObject *)to withProperties:(NSDictionary *)withProperties;  // Copy an object.
- (void) moveTo:(SBObject *)to;  // Move an object to a new location.
- (void) revert;  // Revert a document. Don't use this when you have turned on the auto-relead functionality.
- (NSArray *) joinTo:(id)to continuousSelection:(BOOL)continuousSelection;  // Join selection(s).
- (NSRect) getBoundsForOn:(SkimPage *)on for:(SkimPDFDisplayBox)for_;  // Get the bounds of a page, note, or selection.
- (SBObject *) getTextForOn:(SkimPage *)on;  // Get the text of a document, page, note, selection, or raw RTF data.
- (NSInteger) getIndexForOn:(SkimPage *)on last:(BOOL)last;  // Get the index of a document, page, note, or selection.
- (NSArray *) getPagesFor;  // Get the pages of a document, page, note, or selection.

@end

// This subdivides the text into words.
@interface SkimWord : SBObject

- (SBElementArray *) characters;
- (SBElementArray *) paragraphs;
- (SBElementArray *) words;
- (SBElementArray *) attributeRuns;
- (SBElementArray *) attachments;

@property (copy) NSColor *color;  // The color of the first character.
@property (copy) NSString *font;  // The name of the font of the first character.
@property NSInteger size;  // The size in points of the first character.
@property (copy) NSAttributedString *RTF;  // The raw RTF data.

- (void) closeSaving:(SkimSaveOptions)saving savingIn:(NSURL *)savingIn;  // Close a document.
- (void) saveIn:(NSURL *)in_ as:(NSString *)as;  // Save a document.
- (void) printWithProperties:(NSDictionary *)withProperties printDialog:(BOOL)printDialog;  // Print a document.
- (void) delete;  // Delete an object.
- (void) duplicateTo:(SBObject *)to withProperties:(NSDictionary *)withProperties;  // Copy an object.
- (void) moveTo:(SBObject *)to;  // Move an object to a new location.
- (void) revert;  // Revert a document. Don't use this when you have turned on the auto-relead functionality.
- (NSArray *) joinTo:(id)to continuousSelection:(BOOL)continuousSelection;  // Join selection(s).
- (NSRect) getBoundsForOn:(SkimPage *)on for:(SkimPDFDisplayBox)for_;  // Get the bounds of a page, note, or selection.
- (SBObject *) getTextForOn:(SkimPage *)on;  // Get the text of a document, page, note, selection, or raw RTF data.
- (NSInteger) getIndexForOn:(SkimPage *)on last:(BOOL)last;  // Get the index of a document, page, note, or selection.
- (NSArray *) getPagesFor;  // Get the pages of a document, page, note, or selection.

@end

// This subdivides the text into chunks that all have the same attributes.
@interface SkimAttributeRun : SBObject

- (SBElementArray *) characters;
- (SBElementArray *) paragraphs;
- (SBElementArray *) words;
- (SBElementArray *) attributeRuns;
- (SBElementArray *) attachments;

@property (copy) NSColor *color;  // The color of the first character.
@property (copy) NSString *font;  // The name of the font of the first character.
@property NSInteger size;  // The size in points of the first character.
@property (copy) NSAttributedString *RTF;  // The raw RTF data.

- (void) closeSaving:(SkimSaveOptions)saving savingIn:(NSURL *)savingIn;  // Close a document.
- (void) saveIn:(NSURL *)in_ as:(NSString *)as;  // Save a document.
- (void) printWithProperties:(NSDictionary *)withProperties printDialog:(BOOL)printDialog;  // Print a document.
- (void) delete;  // Delete an object.
- (void) duplicateTo:(SBObject *)to withProperties:(NSDictionary *)withProperties;  // Copy an object.
- (void) moveTo:(SBObject *)to;  // Move an object to a new location.
- (void) revert;  // Revert a document. Don't use this when you have turned on the auto-relead functionality.
- (NSArray *) joinTo:(id)to continuousSelection:(BOOL)continuousSelection;  // Join selection(s).
- (NSRect) getBoundsForOn:(SkimPage *)on for:(SkimPDFDisplayBox)for_;  // Get the bounds of a page, note, or selection.
- (SBObject *) getTextForOn:(SkimPage *)on;  // Get the text of a document, page, note, selection, or raw RTF data.
- (NSInteger) getIndexForOn:(SkimPage *)on last:(BOOL)last;  // Get the index of a document, page, note, or selection.
- (NSArray *) getPagesFor;  // Get the pages of a document, page, note, or selection.

@end

// Represents an inline text attachment. This class is used mainly for make commands.
@interface SkimAttachment : SkimRichText

@property (copy) NSString *fileName;  // The path to the file for the attachment


@end



/*
 * Skim Suite
 */

// Skim's top level scripting object.
@interface SkimApplication (SkimSuite)

- (SBElementArray *) bookmarks;

@property (copy) NSDictionary *defaultViewSettings;  // The default PDF view settings for normal mode.
@property (copy) NSDictionary *defaultFullScreenViewSettings;  // The default PDF view settings for full screen mode. An empty dictionary indicates that default view settings for normal mode are used.
@property (copy) NSColor *backgroundColor;  // The default background color in normal mode.
@property (copy) NSColor *fullScreenBackgroundColor;  // The default background color in full screen mode.
@property (copy) NSColor *pageBackgroundColor;  // The default page background color.
@property (copy) NSDictionary *defaultNoteColors;  // The default colors used for new notes.
@property (copy) NSDictionary *defaultNoteLineWidths;  // The default line width used for new notes.
@property (copy) NSDictionary *defaultNoteLineStyles;  // The default line styles used for new notes.
@property (copy) NSDictionary *defaultNoteDashPatterns;  // The default dash patterns used for new notes.
@property SkimLineEndingStyle defaultNoteStartLineStyle;  // The default start line style used for new lines.
@property SkimLineEndingStyle defaultNoteEndLineStyle;  // The default end line style used for new lines.
@property (copy) NSDictionary *defaultNoteFontNames;  // The default font names used for new notes.
@property (copy) NSDictionary *defaultNoteFontSizes;  // The default font sizes used for new notes.
@property (copy) NSColor *defaultTextNoteFontColor;  // The default font color used for new text notes.
@property SkimTextAlignment defaultTextNoteAlignment;  // The default alignment used for new text notes.
@property SkimIconType defaultNoteIconType;  // The default icon type used for new anchored notes.

@end

// A Skim document.
@interface SkimDocument (SkimSuite)

- (SBElementArray *) pages;
- (SBElementArray *) notes;

@property (copy) SkimPage *currentPage;  // The current page.
@property (copy) SkimNote *activeNote;  // The currently selected note.
@property (copy) NSArray *selection;  // The selection of the document.
@property NSRect selectionBounds;  // The bounds of the selection from the selection tool of the document (left, top, right, bottom).
@property (copy) SkimPage *selectionPage;  // The page for the selection from the selection tool of the document.
@property (copy) NSDictionary *viewSettings;  // The PDF view settings.
@property (copy, readonly) NSDictionary *info;  // The attributes of the PDF document.
@property (readonly) BOOL containingPDF;  // Is this a PDF document?

@end

// A window.
@interface SkimWindow (SkimSuite)


@end

// A page.
@interface SkimPage : SBObject

- (SBElementArray *) notes;

@property (readonly) NSInteger index;  // The index of the page.
@property (copy, readonly) NSString *label;  // The label of the page.
@property NSRect bounds;  // The bounding rectangle for the crop box of the page (left, top, right, bottom).
@property NSRect mediaBounds;  // The bounding rectangle for the media box of the page (left, top, right, bottom).
@property (readonly) NSRect contentBounds;  // The bounding rectangle for the foreground of the page (left, top, right, bottom).
@property (copy, readonly) NSArray *lineBounds;  // The bounding rectangles for the lines on the page (left, top, right, bottom).
@property NSInteger rotation;  // The rotation of the page. Must be one of 0, 90, 180, or 270.

- (void) closeSaving:(SkimSaveOptions)saving savingIn:(NSURL *)savingIn;  // Close a document.
- (void) saveIn:(NSURL *)in_ as:(NSString *)as;  // Save a document.
- (void) printWithProperties:(NSDictionary *)withProperties printDialog:(BOOL)printDialog;  // Print a document.
- (void) delete;  // Delete an object.
- (void) duplicateTo:(SBObject *)to withProperties:(NSDictionary *)withProperties;  // Copy an object.
- (void) moveTo:(SBObject *)to;  // Move an object to a new location.
- (void) revert;  // Revert a document. Don't use this when you have turned on the auto-relead functionality.
- (NSArray *) joinTo:(id)to continuousSelection:(BOOL)continuousSelection;  // Join selection(s).
- (NSRect) getBoundsForOn:(SkimPage *)on for:(SkimPDFDisplayBox)for_;  // Get the bounds of a page, note, or selection.
- (SBObject *) getTextForOn:(SkimPage *)on;  // Get the text of a document, page, note, selection, or raw RTF data.
- (NSInteger) getIndexForOn:(SkimPage *)on last:(BOOL)last;  // Get the index of a document, page, note, or selection.
- (NSArray *) getPagesFor;  // Get the pages of a document, page, note, or selection.
- (id) grabFor:(NSRect)for_ as:(NSNumber *)as;  // Grab a picture from a page.
- (id) formatUsingTemplate:(id)usingTemplate to:(NSURL *)to;  // Format an object using a template.

@end

// A note.
@interface SkimNote : SBObject


@property (readonly) SkimNoteType type;  // The type of note.
@property (copy) NSColor *color;  // The color of the note. Can be given as a list of (red, green, blue, alpha), a standard color (e.g. red), or a default color (e.g. text note color).
@property (copy, readonly) SkimPage *page;  // The page for the note.
@property NSRect bounds;  // The bounding rectangle of the note (left, top, right, bottom).
@property (copy, readonly) NSDate *modificationDate;  // The modification date of a note.
@property (copy) NSString *userName;  // The user name of the creator of a note.
@property (copy) NSString *fontName;  // The font name of a text note.
@property NSInteger fontSize;  // The font size in points of a text note.
@property SkimTextAlignment alignment;  // The text alignment of a text note.
@property (copy) NSColor *fontColor;  // The text color of a text note.
@property (copy) SkimRichText *extendedText;  // The rich extended text of an anchored note.
@property SkimIconType icon;  // The icon type of an anchored note.
@property (copy) NSColor *interiorColor;  // The fill color of a circle, box, or line.
@property double lineWidth;  // The line width of the note.
@property SkimNoteLineStyle lineStyle;  // The line style of the note.
@property (copy) NSArray *dashPattern;  // The dash pattern of the border of the note or of the line, a list of numbers.
@property NSPoint startPoint;  // The start point of a line (left, top).
@property NSPoint endPoint;  // The end point of a line (left, top).
@property SkimLineEndingStyle startLineStyle;  // The start line style of a line.
@property SkimLineEndingStyle endLineStyle;  // The end line style of a line.
@property (copy, readonly) NSArray *selection;  // The selection for a highlight, underline, or strike out markup.
@property (copy, readonly) NSArray *pathList;  // The list of paths for a freehand or markup note.

- (void) closeSaving:(SkimSaveOptions)saving savingIn:(NSURL *)savingIn;  // Close a document.
- (void) saveIn:(NSURL *)in_ as:(NSString *)as;  // Save a document.
- (void) printWithProperties:(NSDictionary *)withProperties printDialog:(BOOL)printDialog;  // Print a document.
- (void) delete;  // Delete an object.
- (void) duplicateTo:(SBObject *)to withProperties:(NSDictionary *)withProperties;  // Copy an object.
- (void) moveTo:(SBObject *)to;  // Move an object to a new location.
- (void) revert;  // Revert a document. Don't use this when you have turned on the auto-relead functionality.
- (NSArray *) joinTo:(id)to continuousSelection:(BOOL)continuousSelection;  // Join selection(s).
- (NSRect) getBoundsForOn:(SkimPage *)on for:(SkimPDFDisplayBox)for_;  // Get the bounds of a page, note, or selection.
- (SBObject *) getTextForOn:(SkimPage *)on;  // Get the text of a document, page, note, selection, or raw RTF data.
- (NSInteger) getIndexForOn:(SkimPage *)on last:(BOOL)last;  // Get the index of a document, page, note, or selection.
- (NSArray *) getPagesFor;  // Get the pages of a document, page, note, or selection.
- (id) formatUsingTemplate:(id)usingTemplate to:(NSURL *)to;  // Format an object using a template.

@end

// A bookmark.
@interface SkimBookmark : SBObject

- (SBElementArray *) bookmarks;

@property (copy, readonly) NSArray *entireContents;  // The entire contents of the bookmark, including the contents of its children
@property (copy) NSString *name;  // The name for the bookmark.
@property (readonly) SkimBookmarkType type;  // The bookmark type.
@property NSInteger pageIndex;  // The page index for the bookmark.
@property (copy, readonly) SkimBookmark *container;  // The folder or session bookmark containing this bookmark.

- (void) closeSaving:(SkimSaveOptions)saving savingIn:(NSURL *)savingIn;  // Close a document.
- (void) saveIn:(NSURL *)in_ as:(NSString *)as;  // Save a document.
- (void) printWithProperties:(NSDictionary *)withProperties printDialog:(BOOL)printDialog;  // Print a document.
- (void) delete;  // Delete an object.
- (void) duplicateTo:(SBObject *)to withProperties:(NSDictionary *)withProperties;  // Copy an object.
- (void) moveTo:(SBObject *)to;  // Move an object to a new location.
- (void) revert;  // Revert a document. Don't use this when you have turned on the auto-relead functionality.
- (NSArray *) joinTo:(id)to continuousSelection:(BOOL)continuousSelection;  // Join selection(s).
- (NSRect) getBoundsForOn:(SkimPage *)on for:(SkimPDFDisplayBox)for_;  // Get the bounds of a page, note, or selection.
- (SBObject *) getTextForOn:(SkimPage *)on;  // Get the text of a document, page, note, selection, or raw RTF data.
- (NSInteger) getIndexForOn:(SkimPage *)on last:(BOOL)last;  // Get the index of a document, page, note, or selection.
- (NSArray *) getPagesFor;  // Get the pages of a document, page, note, or selection.

@end

// A list of points.
@interface SkimPointList : SBObject

- (void) closeSaving:(SkimSaveOptions)saving savingIn:(NSURL *)savingIn;  // Close a document.
- (void) saveIn:(NSURL *)in_ as:(NSString *)as;  // Save a document.
- (void) printWithProperties:(NSDictionary *)withProperties printDialog:(BOOL)printDialog;  // Print a document.
- (void) delete;  // Delete an object.
- (void) duplicateTo:(SBObject *)to withProperties:(NSDictionary *)withProperties;  // Copy an object.
- (void) moveTo:(SBObject *)to;  // Move an object to a new location.
- (void) revert;  // Revert a document. Don't use this when you have turned on the auto-relead functionality.
- (NSArray *) joinTo:(id)to continuousSelection:(BOOL)continuousSelection;  // Join selection(s).
- (NSRect) getBoundsForOn:(SkimPage *)on for:(SkimPDFDisplayBox)for_;  // Get the bounds of a page, note, or selection.
- (SBObject *) getTextForOn:(SkimPage *)on;  // Get the text of a document, page, note, selection, or raw RTF data.
- (NSInteger) getIndexForOn:(SkimPage *)on last:(BOOL)last;  // Get the index of a document, page, note, or selection.
- (NSArray *) getPagesFor;  // Get the pages of a document, page, note, or selection.

@end

// A line in a TeX source document.
@interface SkimTeXLine : SBObject

@property (readonly) NSInteger index;  // the line number.

- (void) closeSaving:(SkimSaveOptions)saving savingIn:(NSURL *)savingIn;  // Close a document.
- (void) saveIn:(NSURL *)in_ as:(NSString *)as;  // Save a document.
- (void) printWithProperties:(NSDictionary *)withProperties printDialog:(BOOL)printDialog;  // Print a document.
- (void) delete;  // Delete an object.
- (void) duplicateTo:(SBObject *)to withProperties:(NSDictionary *)withProperties;  // Copy an object.
- (void) moveTo:(SBObject *)to;  // Move an object to a new location.
- (void) revert;  // Revert a document. Don't use this when you have turned on the auto-relead functionality.
- (NSArray *) joinTo:(id)to continuousSelection:(BOOL)continuousSelection;  // Join selection(s).
- (NSRect) getBoundsForOn:(SkimPage *)on for:(SkimPDFDisplayBox)for_;  // Get the bounds of a page, note, or selection.
- (SBObject *) getTextForOn:(SkimPage *)on;  // Get the text of a document, page, note, selection, or raw RTF data.
- (NSInteger) getIndexForOn:(SkimPage *)on last:(BOOL)last;  // Get the index of a document, page, note, or selection.
- (NSArray *) getPagesFor;  // Get the pages of a document, page, note, or selection.

@end


#ifndef SVGICONS_H
#define SVGICONS_H

#include <QString>
#include <QMap>

namespace SvgIcons {
    inline const QMap<QString, QString> icons = {
        {"text", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><line x1="4" y1="6" x2="20" y2="6"></line><line x1="4" y1="11" x2="14" y2="11"></line><line x1="4" y1="16" x2="20" y2="16"></line><line x1="4" y1="21" x2="14" y2="21"></line></svg>)svg"},
        {"untagged", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M20.59 13.41l-7.17 7.17a2 2 0 0 1-2.83 0L2 12V2h10l8.59 8.59a2 2 0 0 1 0 2.82z"></path><line x1="7" y1="7" x2="7.01" y2="7"></line><path d="M11 11l4 4m0-4l-4 4" /></svg>)svg"},
        {"tag", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M20.59 13.41l-7.17 7.17a2 2 0 0 1-2.83 0L2 12V2h10l8.59 8.59a2 2 0 0 1 0 2.82z"></path><line x1="7" y1="7" x2="7.01" y2="7"></line></svg>)svg"},
        {"file", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M14 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V8z"></path><polyline points="14 2 14 8 20 8"></polyline><line x1="16" y1="13" x2="8" y2="13"></line><line x1="16" y1="17" x2="8" y2="17"></line><line x1="10" y1="9" x2="8" y2="9"></line></svg>)svg"},
        {"files_multiple", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M20 14V8L14 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h8"></path><polyline points="14 2 14 8 20 8"></polyline><line x1="16" y1="13" x2="8" y2="13"></line><line x1="16" y1="17" x2="8" y2="17"></line><line x1="10" y1="9" x2="8" y2="9"></line><path d="M16 19h6m-3-3v6" stroke-width="3"/></svg>)svg"},
        {"code", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polyline points="16 18 22 12 16 6"></polyline><polyline points="8 6 2 12 8 18"></polyline></svg>)svg"},
        {"link", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M10 13a5 5 0 0 0 7.54.54l3-3a5 5 0 0 0-7.07-7.07l-1.72 1.71"></path><path d="M14 11a5 5 0 0 0-7.54-.54l-3 3a5 5 0 0 0 7.07 7.07l1.71-1.71"></path></svg>)svg"},
        {"image", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="3" y="3" width="18" height="18" rx="2" ry="2"></rect><circle cx="8.5" cy="8.5" r="1.5"></circle><polyline points="21 15 16 10 5 21"></polyline></svg>)svg"},
        {"branch", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="5" r="3"></circle><path d="M12 8v5"></path><path d="M12 13l-5 4"></path><path d="M12 13l5 4"></path><circle cx="7" cy="19" r="3"></circle><circle cx="17" cy="19" r="3"></circle></svg>)svg"},
        {"category", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="8" y="2" width="8" height="6" rx="1"></rect><path d="M12 8 v3"></path><path d="M12 11 h-6"></path><path d="M12 11 h6"></path><rect x="2" y="13" width="8" height="5" rx="1"></rect><rect x="14" y="13" width="8" height="5" rx="1"></rect><circle cx="12" cy="5" r="1" fill="currentColor"></circle><circle cx="6" cy="15.5" r="1" fill="currentColor"></circle><circle cx="18" cy="15.5" r="1" fill="currentColor"></circle></svg>)svg"},
        {"trash", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polyline points="3 6 5 6 21 6" /><path d="M19 6v14a2 2 0 0 1-2 2H7a2 2 0 0 1-2-2V6m3 0V4a2 2 0 0 1 2-2h4a2 2 0 0 1 2 2v2" /><line x1="10" y1="11" x2="10" y2="17" /><line x1="14" y1="11" x2="14" y2="17" /></svg>)svg"},
        {"search", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="11" cy="11" r="8"></circle><line x1="21" y1="21" x2="16.65" y2="16.65"></line></svg>)svg"},
        {"add", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><line x1="12" y1="5" x2="12" y2="19"></line><line x1="5" y1="12" x2="19" y2="12"></line></svg>)svg"},
        {"edit", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M11 4H4a2 2 0 0 0-2 2v14a2 2 0 0 0 2 2h14a2 2 0 0 0 2-2v-7"></path><path d="M18.5 2.5a2.121 2.121 0 0 1 3 3L12 15l-4 1 1-4 9.5-9.5z"></path></svg>)svg"},
        {"bookmark", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M19 21l-7-5-7 5V5a2 2 0 0 1 2-2h10a2 2 0 0 1 2 2z"></path></svg>)svg"},
        {"Favorite", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polygon points="12 2 15.09 8.26 22 9.27 17 14.14 18.18 21.02 12 17.77 5.82 21.02 7 14.14 2 9.27 8.91 8.26 12 2"></polygon></svg>)svg"},
        {"location", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M21 10c0 7-9 13-9 13s-9-6-9-13a9 9 0 0 1 18 0z"></path><circle cx="12" cy="10" r="3"></circle></svg>)svg"},
        {"pin", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><path d="M16 9V4h1c.55 0 1-.45 1-1s-.45-1-1-1H7c-.55 0-1 .45-1 1s.45 1 1 1h1v5c0 1.66-1.34 3-3 3v2h5.97v7l1.03 1 1.03-1v-7H19v-2c-1.66 0-3-1.34-3-3z"></path></svg>)svg"},
        {"lock", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="3" y="11" width="18" height="11" rx="2"/><path d="M7 11V7a5 5 0 0 1 10 0v4"/><circle cx="7" cy="17" r="0.7" fill="currentColor"/><circle cx="12" cy="17" r="0.7" fill="currentColor"/><circle cx="17" cy="17" r="0.7" fill="currentColor"/></svg>)svg"},
        {"lock_secure", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="3" y="11" width="18" height="11" rx="2"/><path d="M7 11V7a5 5 0 0 1 10 0v4"/><circle cx="12" cy="15.5" r="2" fill="none" stroke="currentColor" stroke-width="1.5"/><line x1="12" y1="17.5" x2="12" y2="20.5" stroke="currentColor" stroke-width="1.5" stroke-linecap="round"/></svg>)svg"},
        {"password_generator", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
            <path d="M16 11V7a4 4 0 0 0-8 0v4" />
            <rect x="3" y="11" width="13" height="10" rx="2" />
            <rect x="11" y="14" width="11" height="7" rx="3.5" />
            <rect x="13.5" y="16.5" width="1.5" height="1.5" fill="currentColor" stroke="none" />
            <rect x="16.25" y="16.5" width="1.5" height="1.5" fill="currentColor" stroke="none" />
            <rect x="19" y="16.5" width="1.5" height="1.5" fill="currentColor" stroke="none" />
        </svg>)svg"},
        {"message", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M3 15a2 2 0 0 0 2 2h12l4 4V5a2 2 0 0 0-2-2H5a2 2 0 0 0-2 2z"></path><line x1="8" y1="9" x2="16" y2="9"></line><line x1="8" y1="13" x2="14" y2="13"></line></svg>)svg"},
        {"eye", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M1 12s4-8 11-8 11 8 11 8-4 8-11 8-11-8-11-8z"></path><circle cx="12" cy="12" r="3"></circle></svg>)svg"},
        {"toolbox", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="2" y="7" width="20" height="14" rx="2" ry="2"></rect><path d="M6 7V5a2 2 0 0 1 2-2h8a2 2 0 0 1 2 2v2"></path><line x1="12" y1="12" x2="12" y2="16"></line><line x1="8" y1="12" x2="8" y2="16"></line><line x1="16" y1="12" x2="16" y2="16"></line></svg>)svg"},
        {"today", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="3" y="4" width="18" height="18" rx="2" ry="2"></rect><line x1="16" y1="2" x2="16" y2="6"></line><line x1="8" y1="2" x2="8" y2="6"></line><line x1="3" y1="10" x2="21" y2="10"></line></svg>)svg"},
        {"all_data", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><ellipse cx="12" cy="5" rx="9" ry="3"></ellipse><path d="M21 12c0 1.66-4 3-9 3s-9-1.34-9-3"></path><path d="M3 5v14c0 1.66 4 3 9 3s9-1.34 9-3V5"></path></svg>)svg"},
        {"sidebar", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="3" y="3" width="18" height="18" rx="2" ry="2"></rect><line x1="9" y1="3" x2="9" y2="21"></line></svg>)svg"},
        {"sidebar_right", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="3" y="3" width="18" height="18" rx="2" ry="2"></rect><line x1="15" y1="3" x2="15" y2="21"></line></svg>)svg"},
        {"nav_first", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polyline points="11 17 6 12 11 7"></polyline><polyline points="18 17 13 12 18 7"></polyline></svg>)svg"},
        {"nav_prev", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polyline points="15 18 9 12 15 6"></polyline></svg>)svg"},
        {"nav_next", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polyline points="9 18 15 12 9 6"></polyline></svg>)svg"},
        {"nav_last", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polyline points="13 17 18 12 13 7"></polyline><polyline points="6 17 11 12 6 7"></polyline></svg>)svg"},
        {"undo", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M3 7v6h6"></path><path d="M21 17a9 9 0 0 0-9-9 9 9 0 0 0-6 2.3L3 13"></path></svg>)svg"},
        {"coffee", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M18 8h1a4 4 0 0 1 0 8h-1"/><path d="M2 8h16v9a4 4 0 0 1-4 4H6a4 4 0 0 1-4-4V8z"/><line x1="6" y1="1" x2="6" y2="4"/><line x1="10" y1="1" x2="10" y2="4"/><line x1="14" y1="1" x2="14" y2="4"/></svg>)svg"},
        {"grid", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="3" y="3" width="18" height="18" rx="2" ry="2"/><line x1="3" y1="9" x2="21" y2="9"/><line x1="3" y1="15" x2="21" y2="15"/><line x1="9" y1="3" x2="9" y2="21"/><line x1="15" y1="3" x2="15" y2="21"/></svg>)svg"},
        {"book", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M4 19.5A2.5 2.5 0 0 1 6.5 17H20v2H6.5A2.5 2.5 0 0 1 4 19.5z"/><path d="M4 5.5A2.5 2.5 0 0 1 6.5 3H20v2H6.5A2.5 2.5 0 0 1 4 5.5z"/></svg>)svg"},
        {"leaf", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M20 12c0-4.42-3.58-8-8-8S4 7.58 4 12s3.58 8 8 8 8-3.58 8-8z"/><path d="M12 2a10 10 0 0 0-10 10h20a10 10 0 0 0-10-10z"/></svg>)svg"},
        {"book_open", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M2 3h6a4 4 0 0 1 4 4v14a3 3 0 0 0-3-3H2z"/><path d="M22 3h-6a4 4 0 0 0-4 4v14a3 3 0 0 1 3-3h7z"/></svg>)svg"},
        {"redo", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M21 7v6h-6"></path><path d="M3 17a9 9 0 0 1 9-9 9 9 0 0 1 6 2.3l3 2.7"></path></svg>)svg"},
        {"list_ul", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><line x1="8" y1="6" x2="21" y2="6"></line><line x1="8" y1="12" x2="21" y2="12"></line><line x1="8" y1="18" x2="21" y2="18"></line><line x1="3" y1="6" x2="3.01" y2="6"></line><line x1="3" y1="12" x2="3.01" y2="12"></line><line x1="3" y1="18" x2="3.01" y2="18"></line></svg>)svg"},
        {"list_ol", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><line x1="10" y1="6" x2="21" y2="6"></line><line x1="10" y1="12" x2="21" y2="12"></line><line x1="10" y1="18" x2="21" y2="18"></line><path d="M4 6h1v4"></path><path d="M4 10h2"></path><path d="M6 18H4c0-1 2-2 2-3s-1-1.5-2-1"></path></svg>)svg"},
        {"todo", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="3" y="3" width="18" height="18" rx="2" ry="2"></rect><path d="M9 12l2 2 4-4"></path></svg>)svg"},
        {"close", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="3.5" stroke-linecap="round" stroke-linejoin="round"><line x1="18" y1="6" x2="6" y2="18"></line><line x1="6" y1="6" x2="18" y2="18"></line></svg>)svg"},
        {"save", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M19 21H5a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h11l5 5v11a2 2 0 0 1-2 2z"></path><polyline points="17 21 17 13 7 13 7 21"></polyline><polyline points="7 3 7 8 15 8"></polyline></svg>)svg"},
        {"filter", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M9 11l3 3L22 4"/><path d="M21 12v7a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h11"/></svg>)svg"},
        {"select", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polyline points="9 11 12 14 22 4"></polyline><path d="M21 12v7a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h11"></path></svg>)svg"},
        {"grip_diagonal", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="19" cy="19" r="1"></circle><circle cx="19" cy="14" r="1"></circle><circle cx="14" cy="19" r="1"></circle><circle cx="19" cy="9" r="1"></circle><circle cx="14" cy="14" r="1"></circle><circle cx="9" cy="19" r="1"></circle></svg>)svg"},
        {"folder", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M22 19a2 2 0 0 1-2 2H4a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h5l2 3h9a2 2 0 0 1 2 2z"></path></svg>)svg"},
        {"folders_multiple", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M22 14V8a2 2 0 0 0-2-2h-9l-2-3H4a2 2 0 0 0-2 2v14a2 2 0 0 0 2 2h10"></path><path d="M16 19h6m-3-3v6" stroke-width="3"/></svg>)svg"},
        {"file_managed", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M14 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V8z"></path><polyline points="14 2 14 8 20 8"></polyline><path d="M12 18h6v3h-6z" fill="currentColor" stroke="none" /><path d="M12 15h6v1h-6z" fill="currentColor" stroke="none" /></svg>)svg"},
        {"folder_managed", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M22 19a2 2 0 0 1-2 2H4a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h5l2 3h9a2 2 0 0 1 2 2z"></path><path d="M12 18h8v3h-8z" fill="currentColor" stroke="none" /><path d="M12 15h8v1h-8z" fill="currentColor" stroke="none" /></svg>)svg"},
        {"settings", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="3"></circle><path d="M19.4 15a1.65 1.65 0 0 0 .33 1.82l.06.06a2 2 0 0 1 0 2.83 2 2 0 0 1-2.83 0l-.06-.06a1.65 1.65 0 0 0-1.82-.33 1.65 1.65 0 0 0-1 1.51V21a2 2 0 0 1-2 2 2 2 0 0 1-2-2v-.09A1.65 1.65 0 0 0 9 19.4a1.65 1.65 0 0 0-1.82.33l-.06.06a2 2 0 0 1-2.83 0 2 2 0 0 1 0-2.83l.06-.06a1.65 1.65 0 0 0 .33-1.82 1.65 1.65 0 0 0-1.51-1H3a2 2 0 0 1-2-2 2 2 0 0 1 2-2h.09A1.65 1.65 0 0 0 4.6 9a1.65 1.65 0 0 0-.33-1.82l-.06-.06a2 2 0 0 1 0-2.83 2 2 0 0 1 2.83 0l.06.06a1.65 1.65 0 0 0 1.82.33H9a1.65 1.65 0 0 0 1-1.51V3a2 2 0 0 1 2-2 2 2 0 0 1 2 2v.09a1.65 1.65 0 0 0 1 1.51 1.65 1.65 0 0 0 1.82-.33l.06-.06a2 2 0 0 1 2.83 0 2 2 0 0 1 0 2.83l-.06.06a1.65 1.65 0 0 0-.33 1.82V9a1.65 1.65 0 0 0 1.51 1H21a2 2 0 0 1 2 2 2 2 0 0 1-2 2h-.09a1.65 1.65 0 0 0-1.51 1z"></path></svg>)svg"},
        {"calendar", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="3" y="4" width="18" height="18" rx="2" ry="2"></rect><line x1="16" y1="2" x2="16" y2="6"></line><line x1="8" y1="2" x2="8" y2="6"></line><line x1="3" y1="10" x2="21" y2="10"></line></svg>)svg"},
        {"clock", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="10"></circle><polyline points="12 6 12 12 16 14"></polyline></svg>)svg"},
        {"palette", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="13.5" cy="6.5" r="2.5"></circle><circle cx="17.5" cy="10.5" r="2.5"></circle><circle cx="8.5" cy="7.5" r="2.5"></circle><circle cx="6.5" cy="12.5" r="2.5"></circle><path d="M12 2C6.5 2 2 6.5 2 12s4.5 10 10 10c.926 0 1.648-.746 1.648-1.688 0-.437-.18-.835-.437-1.125-.29-.289-.438-.652-.438-1.125a1.64 1.64 0 0 1 1.668-1.668h1.996c3.051 0 5.555-2.503 5.555-5.554C21.965 6.012 17.461 2 12 2z"></path></svg>)svg"},
        {"zap", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polygon points="13 2 3 14 12 14 11 22 21 10 12 10 13 2"></polygon></svg>)svg"},
        {"monitor", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="2" y="3" width="20" height="14" rx="2" ry="2"></rect><line x1="8" y1="21" x2="16" y2="21"></line><line x1="12" y1="17" x2="12" y2="21"></line></svg>)svg"},
        {"power", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M18.36 6.64a9 9 0 1 1-12.73 0"></path><line x1="12" y1="2" x2="12" y2="12"></line></svg>)svg"},
        {"minimize", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><line x1="5" y1="12" x2="19" y2="12"></line></svg>)svg"},
        {"maximize", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="3" y="3" width="18" height="18" rx="2" ry="2"></rect></svg>)svg"},
        {"copy", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="9" y="9" width="13" height="13" rx="2" ry="2"></rect><path d="M5 15H4a2 2 0 0 1-2-2V4a2 2 0 0 1 2-2h9a2 2 0 0 1 2 2v1"></path></svg>)svg"},
        {"pin_vertical", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><path d="M16 9V4h1c.55 0 1-.45 1-1s-.45-1-1-1H7c-.55 0-1 .45-1 1s.45 1 1 1h1v5c0 1.66-1.34 3-3 3v2h5.97v7l1.03 1 1.03-1v-7H19v-2c-1.66 0-3-1.34-3-3z"></path></svg>)svg"},
        {"pin_tilted", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><g transform="rotate(45 12 12)"><path d="M16 9V4h1c.55 0 1-.45 1-1s-.45-1-1-1H7c-.55 0-1 .45-1 1s.45 1 1 1h1v5c0 1.66-1.34 3-3 3v2h5.97v7l1.03 1 1.03-1v-7H19v-2c-1.66 0-3-1.34-3-3z"></path></g></svg>)svg"},
        {"Favorite_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><polygon points="12 2 15.09 8.26 22 9.27 17 14.14 18.18 21.02 12 17.77 5.82 21.02 7 14.14 2 9.27 8.91 8.26 12 2"></polygon></svg>)svg"},
        {"bookmark_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><path d="M19 21l-7-5-7 5V5a2 2 0 0 1 2-2h10a2 2 0 0 1 2 2z"></path></svg>)svg"},
        {"circle_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><circle cx="12" cy="12" r="8"></circle></svg>)svg"},
        {"edit_clear", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M17.5 19H9a2 2 0 0 1-2-2V7a2 2 0 0 1 2-2h8.5L22 12l-4.5 7z"></path><path d="M12 9l4 4"></path><path d="M16 9l-4 4"></path></svg>)svg"},
        {"no_color", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="10"></circle><line x1="4.93" y1="4.93" x2="19.07" y2="19.07"></line></svg>)svg"},
        {"random_color", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M21 16V8a2 2 0 0 0-1-1.73l-7-4a2 2 0 0 0-2 0l-7 4A2 2 0 0 0 3 8v8a2 2 0 0 0 1 1.73l7 4a2 2 0 0 0 2 0l7-4A2 2 0 0 0 21 16z"></path><polyline points="3.27 6.96 12 12.01 20.73 6.96"></polyline><line x1="12" y1="22.08" x2="12" y2="12"></line></svg>)svg"},
        {"screen_picker", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="m18 2 4 4"/><path d="m17 7 3-3"/><path d="M19 9 8.7 19.3c-1 1-2.5 1-3.4 0l-.6-.6c-1-1-1-2.5 0-3.4L15 5"/><path d="m8.5 12 3 3"/><path d="m11 9.5 3 3"/><path d="m5 19-3 3"/><path d="m14 4 6 6"/></svg>)svg"},
        {"pixel_ruler", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="2" y="7" width="20" height="10" rx="2" ry="2" transform="rotate(45 12 12)"/><path d="m8.5 9.5 1 1"/><path d="m11 12 1 1"/><path d="m13.5 14.5 1 1"/><path d="m16 17 1 1"/></svg>)svg"},
        {"ruler_bounds", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M4 8V4h4m8 0h4v4m0 8v4h-4M8 20H4v-4"/></svg>)svg"},
        {"ruler_spacing", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M12 4v16m-8-8h16"/></svg>)svg"},
        {"ruler_hor", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><line x1="2" y1="12" x2="22" y2="12"/><line x1="2" y1="8" x2="2" y2="16"/><line x1="22" y1="8" x2="22" y2="16"/></svg>)svg"},
        {"ruler_ver", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><line x1="12" y1="2" x2="12" y2="22"/><line x1="8" y1="2" x2="16" y2="2"/><line x1="8" y1="22" x2="16" y2="22"/></svg>)svg"},
        {"screenshot_rect", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="3" y="3" width="18" height="18" rx="2" ry="2"/></svg>)svg"},
        {"screenshot_fill", R"svg(<svg viewBox="0 0 24 24" fill="currentColor"><rect x="3" y="3" width="18" height="18" rx="2" ry="2"/></svg>)svg"},
        {"screenshot_ellipse", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="10"/></svg>)svg"},
        {"screenshot_arrow", R"svg(<svg viewBox="0 0 24 24" fill="currentColor"><path d="M22 2L11 5L14 8L4 18L6 20L16 10L19 13L22 2Z"/></svg>)svg"},
        {"screenshot_pen", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M12 19l7-7 3 3-7 7-3-3z"/><path d="M18 13l-1.5-7.5L2 2l3.5 14.5L13 18l5-5z"/></svg>)svg"},
        {"screenshot_marker", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="10"/><text x="12" y="16" text-anchor="middle" font-size="12" font-weight="bold" fill="currentColor">1</text></svg>)svg"},
        {"screenshot_mosaic", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="3" y="3" width="6" height="6"/><rect x="9" y="3" width="6" height="6"/><rect x="15" y="3" width="6" height="6"/><rect x="3" y="9" width="6" height="6"/><rect x="9" y="9" width="6" height="6"/><rect x="15" y="9" width="6" height="6"/><rect x="3" y="15" width="6" height="6"/><rect x="9" y="15" width="6" height="6"/><rect x="15" y="15" width="6" height="6"/></svg>)svg"},
        {"screenshot_confirm", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polyline points="20 6 9 17 4 12"/></svg>)svg"},
        {"screenshot_text", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polyline points="4 7 4 4 20 4 20 7"/><line x1="9" y1="20" x2="15" y2="20"/><line x1="12" y1="4" x2="12" y2="20"/></svg>)svg"},
        {"screenshot_line", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><line x1="5" y1="19" x2="19" y2="5"></line></svg>)svg"},
        {"screenshot_save", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M19 21H5a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h11l5 5v11a2 2 0 0 1-2 2z"></path><polyline points="17 21 17 13 7 13 7 21"></polyline><polyline points="7 3 7 8 15 8"></polyline></svg>)svg"},
        {"screenshot_copy", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="9" y="9" width="13" height="13" rx="2" ry="2"></rect><path d="M5 15H4a2 2 0 0 1-2-2V4a2 2 0 0 1 2-2h9a2 2 0 0 1 2 2v1"></path></svg>)svg"},
        {"screenshot_close", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><line x1="18" y1="6" x2="6" y2="18"></line><line x1="6" y1="6" x2="18" y2="18"></line></svg>)svg"},
        {"screenshot_eraser", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="m7 21-4.3-4.3c-1-1-1-2.5 0-3.4l9.6-9.6c1-1 2.5-1 3.4 0l5.6 5.6c1 1 1 2.5 0 3.4L13 21"/><path d="M22 21H7"/><path d="m5 11 9 9"/></svg>)svg"},
        {"screenshot_pin", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M16 12V6H8v6l-2 2v2h5v8l1 1 1-1v-8h5v-2l-2-2z"></path></svg>)svg"},
        {"screenshot_ocr", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M7 3H5a2 2 0 0 0-2 2v2M17 3h2a2 2 0 0 1 2 2v2M7 21H5a2 2 0 0 1-2-2v-2M17 21h2a2 2 0 0 0 2-2v-2M8 8h8M8 12h8M8 16h5"/></svg>)svg"},
        {"bold", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="3" stroke-linecap="round" stroke-linejoin="round"><path d="M6 4h8a4 4 0 0 1 4 4 4 4 0 0 1-4 4H6z"></path><path d="M6 12h9a4 4 0 0 1 4 4 4 4 0 0 1-4 4H6z"></path></svg>)svg"},
        {"italic", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="3" stroke-linecap="round" stroke-linejoin="round"><line x1="19" y1="4" x2="10" y2="4"></line><line x1="14" y1="20" x2="5" y2="20"></line><line x1="15" y1="4" x2="9" y2="20"></line></svg>)svg"},
        {"color_wheel", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="10"></circle><path d="M12 2v20M2 12h20M12 2a10 10 0 0 1 7.07 17.07M12 2A10 10 0 0 0 4.93 19.07"/></svg>)svg"},
        {"typesetting", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M4 6h16M4 12h10M4 18h16"/></svg>)svg"},
        {"find_keyword", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M14 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V8z"></path><polyline points="14 2 14 8 20 8"></polyline><circle cx="11.5" cy="14.5" r="2.5"></circle><line x1="13.5" y1="16.5" x2="15.5" y2="18.5"></line></svg>)svg"},
        {"swap", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M7 10l5-5 5 5M17 14l-5 5-5-5M12 5v14"/></svg>)svg"},
        {"merge", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M8 6h3a4 4 0 0 1 4 4v2"/><path d="M8 18h3a4 4 0 0 0 4-4v-2"/><path d="M15 12h6"/><polyline points="18 9 21 12 18 15"/></svg>)svg"},
        {"cut", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="6" cy="6" r="3"></circle><circle cx="6" cy="18" r="3"></circle><line x1="20" y1="4" x2="8.12" y2="15.88"></line><line x1="14.47" y1="14.48" x2="20" y2="20"></line><line x1="8.12" y1="8.12" x2="12" y2="12"></line></svg>)svg"},
        {"rotate", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round"><path d="M18 4H10a4 4 0 0 0-4 4v12"/><polyline points="3 17 6 20 9 17"/><path d="M6 20h8a4 4 0 0 0 4-4V4"/><polyline points="21 7 18 4 15 7"/></svg>)svg"},
        {"menu_dots", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="1"/><circle cx="12" cy="5" r="1"/><circle cx="12" cy="19" r="1"/></svg>)svg"},
        {"move", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polyline points="5 9 2 12 5 15"/><polyline points="9 5 12 2 15 5"/><polyline points="15 19 12 22 9 19"/><polyline points="19 9 22 12 19 15"/><line x1="2" y1="12" x2="22" y2="12"/><line x1="12" y1="2" x2="12" y2="22"/></svg>)svg"},
        {"help", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="10"></circle><path d="M9.09 9a3 3 0 0 1 5.83 1c0 2-3 3-3 3"></path><line x1="12" y1="17" x2="12.01" y2="17"></line></svg>)svg"},
        {"scan", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M3 7V5a2 2 0 0 1 2-2h2"></path><path d="M17 3h2a2 2 0 0 1 2 2v2"></path><path d="M21 17v2a2 2 0 0 1-2 2h-2"></path><path d="M7 21H5a2 2 0 0 1-2-2v-2"></path><line x1="7" y1="12" x2="17" y2="12"></line></svg>)svg"},
        {"refresh", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M21 2v6h-6"/><path d="M3 12a9 9 0 0 1 15-6.7L21 8"/><path d="M3 22v-6h6"/><path d="M21 12a9 9 0 0 1-15 6.7L3 16"/></svg>)svg"},
        {"camera", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M23 19a2 2 0 0 1-2 2H3a2 2 0 0 1-2-2V8a2 2 0 0 1 2-2h4l2-3h6l2 3h4a2 2 0 0 1 2 2z"></path><circle cx="12" cy="13" r="4"></circle></svg>)svg"},
        {"ball_on", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="9"></circle><path d="M12 8v4l3 2"></path></svg>)svg"},
        {"paint_bucket", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
            <path d="m19 11-8-8-8.6 8.6a2 2 0 0 0 0 2.8l5.2 5.2a2 2 0 0 0 2.8 0L19 11Z"/>
            <path d="m5 2 5 5"/>
            <path d="m2 13 5 5"/>
            <path d="M22 20a2 2 0 1 1-4 0c0-1.6 1.7-2.4 2-4 .3 1.6 2 2.4 2 4Z" fill="currentColor" stroke="none"/>
        </svg>)svg"},
        {"switch_on", R"svg(<svg viewBox="0 0 24 24"><rect x="2" y="5" width="20" height="14" rx="7" fill="white" /><rect x="3.5" y="6.5" width="17" height="11" rx="5.5" fill="currentColor" /><circle cx="15" cy="12" r="3.5" fill="white" /></svg>)svg"},
        {"switch_off", R"svg(<svg viewBox="0 0 24 24"><rect x="2" y="5" width="20" height="14" rx="7" fill="white" /><rect x="3.5" y="6.5" width="17" height="11" rx="5.5" fill="currentColor" /><circle cx="9" cy="12" r="3.5" fill="white" /></svg>)svg"},
        {"arrow_up", R"svg(<svg viewBox="0 0 24 24" fill="currentColor"><path d="M12 5 L4 19 L20 19 Z"/></svg>)svg"},
        {"arrow_down", R"svg(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="3.5" stroke-linecap="round" stroke-linejoin="round"><polyline points="7 10 12 15 17 10"/></svg>)svg"},
        {"filter_funnel", R"svg(<svg viewBox="0 0 24 24" fill="currentColor"><rect x="3" y="3" width="18" height="3.5" rx="1.5"/><path d="M4 8.5 L10 15.5 V21 L14 18 V15.5 L20 8.5 Z"/></svg>)svg"},
        {"file_import", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M14 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V8z"></path><polyline points="14 2 14 8 20 8"></polyline><path d="M12 12v6m-3-3 3 3 3-3"/></svg>)svg"},
        {"folder_import", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M22 19a2 2 0 0 1-2 2H4a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h5l2 3h9a2 2 0 0 1 2 2z"></path><path d="M12 12v6m-3-3 3 3 3-3"/></svg>)svg"},
        {"batch_import", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M7 8H5a2 2 0 0 0-2 2v10a2 2 0 0 0 2 2h10a2 2 0 0 0 2-2v-2"/><rect x="8" y="2" width="13" height="13" rx="2" ry="2"/><path d="M14.5 6v6m-2.5-3 2.5 3 2.5-3"/></svg>)svg"},
        {"file_export", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M14 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V8z"></path><polyline points="14 2 14 8 20 8"></polyline><path d="M12 18v-6m-3 3 3-3 3 3"/></svg>)svg"},
        {"home", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M3 9l9-7 9 7v11a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2z"/><polyline points="9 22 9 12 15 12 15 22"/></svg>)svg"},
        {"menu", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><line x1="3" y1="6" x2="21" y2="6"/><line x1="3" y1="12" x2="21" y2="12"/><line x1="3" y1="18" x2="21" y2="18"/></svg>)svg"},
        {"layout", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="3" y="3" width="18" height="18" rx="2"/><path d="M3 9h18M9 21V9"/></svg>)svg"},
        {"column_view", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="3" y="3" width="18" height="18" rx="2"/><line x1="9" y1="3" x2="9" y2="21"/><line x1="15" y1="3" x2="15" y2="21"/></svg>)svg"},
        {"columns", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M12 3V21M7.8 3H16.2C17.8802 3 18.7202 3 19.362 3.32698C19.9265 3.6146 20.3854 4.07354 20.673 4.63803C21 5.27976 21 6.11984 21 7.8V16.2C21 17.8802 21 18.7202 20.673 19.362C20.3854 19.9265 19.9265 20.3854 19.362 20.673C18.7202 21 17.8802 21 16.2 21H7.8C6.11984 21 5.27976 21 4.63803 20.673C4.07354 20.3854 3.6146 19.9265 3.32698 19.362C3 18.7202 3 17.8802 3 16.2V7.8C3 6.11984 3 5.27976 3.32698 4.63803C3.6146 4.07354 4.07354 3.6146 4.63803 3.32698C5.27976 3 6.11984 3 7.8 3Z"/></svg>)svg"},
        {"columns-02-svgrepo-com.svg", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M12 3V21M7.8 3H16.2C17.8802 3 18.7202 3 19.362 3.32698C19.9265 3.6146 20.3854 4.07354 20.673 4.63803C21 5.27976 21 6.11984 21 7.8V16.2C21 17.8802 21 18.7202 20.673 19.362C20.3854 19.9265 19.9265 20.3854 19.362 20.673C18.7202 21 17.8802 21 16.2 21H7.8C6.11984 21 5.27976 21 4.63803 20.673C4.07354 20.3854 3.6146 19.9265 3.32698 19.362C3 18.7202 3 17.8802 3 16.2V7.8C3 6.11984 3 5.27976 3.32698 4.63803C3.6146 4.07354 4.07354 3.6146 4.63803 3.32698C5.27976 3 6.11984 3 7.8 3Z"/></svg>)svg"},
        {"columns-02-svgrepo-com", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M12 3V21M7.8 3H16.2C17.8802 3 18.7202 3 19.362 3.32698C19.9265 3.6146 20.3854 4.07354 20.673 4.63803C21 5.27976 21 6.11984 21 7.8V16.2C21 17.8802 21 18.7202 20.673 19.362C20.3854 19.9265 19.9265 20.3854 19.362 20.673C18.7202 21 17.8802 21 16.2 21H7.8C6.11984 21 5.27976 21 4.63803 20.673C4.07354 20.3854 3.6146 19.9265 3.32698 19.362C3 18.7202 3 17.8802 3 16.2V7.8C3 6.11984 3 5.27976 3.32698 4.63803C3.6146 4.07354 4.07354 3.6146 4.63803 3.32698C5.27976 3 6.11984 3 7.8 3Z"/></svg>)svg"},
        {"table", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="3" y="3" width="18" height="18" rx="2"/><path d="M3 9h18M3 15h18M9 3v18M15 3v18"/></svg>)svg"},
        {"zoom_in", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="11" cy="11" r="8"/><line x1="21" y1="21" x2="16.65" y2="16.65"/><line x1="11" y1="8" x2="11" y2="14"/><line x1="8" y1="11" x2="14" y2="11"/></svg>)svg"},
        {"zoom_out", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="11" cy="11" r="8"/><line x1="21" y1="21" x2="16.65" y2="16.65"/><line x1="8" y1="11" x2="14" y2="11"/></svg>)svg"},
        {"maximize_2", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polyline points="15 3 21 3 21 9"/><polyline points="9 21 3 21 3 15"/><line x1="21" y1="3" x2="14" y2="10"/><line x1="3" y1="21" x2="10" y2="14"/></svg>)svg"},
        {"minimize_2", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polyline points="4 14 10 14 10 20"/><polyline points="20 10 14 10 14 4"/><line x1="10" y1="14" x2="3" y2="21"/><line x1="21" y1="3" x2="14" y2="10"/></svg>)svg"},
        {"external_link", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M18 13v6a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2V8a2 2 0 0 1 2-2h6"/><polyline points="15 3 21 3 21 9"/><line x1="10" y1="14" x2="21" y2="3"/></svg>)svg"},
        {"log_in", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M15 3h4a2 2 0 0 1 2 2v14a2 2 0 0 1-2 2h-4"/><polyline points="10 17 15 12 10 7"/><line x1="15" y1="12" x2="3" y2="12"/></svg>)svg"},
        {"log_out", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M9 21H5a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h4"/><polyline points="16 17 21 12 16 7"/><line x1="21" y1="12" x2="9" y2="12"/></svg>)svg"},
        {"sliders", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><line x1="4" y1="21" x2="4" y2="14"/><line x1="4" y1="10" x2="4" y2="3"/><line x1="12" y1="21" x2="12" y2="12"/><line x1="12" y1="8" x2="12" y2="3"/><line x1="20" y1="21" x2="20" y2="16"/><line x1="20" y1="12" x2="20" y2="3"/><line x1="1" y1="14" x2="7" y2="14"/><line x1="9" y1="8" x2="15" y2="8"/><line x1="17" y1="16" x2="23" y2="16"/></svg>)svg"},
        {"crop", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M6.13 1L6 16a2 2 0 0 0 2 2h15"/><path d="M1 6.13l15-.13a2 2 0 0 1 2 2v15"/></svg>)svg"},
        {"user", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M20 21v-2a4 4 0 0 0-4-4H8a4 4 0 0 0-4 4v2"/><circle cx="12" cy="7" r="4"/></svg>)svg"},
        {"users", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M17 21v-2a4 4 0 0 0-4-4H5a4 4 0 0 0-4 4v2"/><circle cx="9" cy="7" r="4"/><path d="M23 21v-2a4 4 0 0 0-3-3.87"/><path d="M16 3.13a4 4 0 0 1 0 7.75"/></svg>)svg"},
        {"user_plus", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M16 21v-2a4 4 0 0 0-4-4H6a4 4 0 0 0-4 4v2"/><circle cx="9" cy="7" r="4"/><line x1="19" y1="8" x2="19" y2="14"/><line x1="22" y1="11" x2="16" y2="11"/></svg>)svg"},
        {"user_minus", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M16 21v-2a4 4 0 0 0-4-4H6a4 4 0 0 0-4 4v2"/><circle cx="9" cy="7" r="4"/><line x1="22" y1="11" x2="16" y2="11"/></svg>)svg"},
        {"user_check", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M16 21v-2a4 4 0 0 0-4-4H6a4 4 0 0 0-4 4v2"/><circle cx="9" cy="7" r="4"/><polyline points="16 11 18 13 22 9"/></svg>)svg"},
        {"contact", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M17 21v-2a4 4 0 0 0-4-4H5a4 4 0 0 0-4 4v2"/><circle cx="9" cy="7" r="4"/><path d="M23 11l-4 4-2-2"/></svg>)svg"},
        {"heart", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M20.84 4.61a5.5 5.5 0 0 0-7.78 0L12 5.67l-1.06-1.06a5.5 5.5 0 0 0-7.78 7.78l1.06 1.06L12 21.23l7.78-7.78 1.06-1.06a5.5 5.5 0 0 0 0-7.78z"/></svg>)svg"},
        {"heart_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><path d="M20.84 4.61a5.5 5.5 0 0 0-7.78 0L12 5.67l-1.06-1.06a5.5 5.5 0 0 0-7.78 7.78l1.06 1.06L12 21.23l7.78-7.78 1.06-1.06a5.5 5.5 0 0 0 0-7.78z"/></svg>)svg"},
        {"thumbs_up", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M14 9V5a3 3 0 0 0-3-3l-4 9v11h11.28a2 2 0 0 0 2-1.7l1.38-9a2 2 0 0 0-2-2.3z"/><path d="M7 22H4a2 2 0 0 1-2-2v-7a2 2 0 0 1 2-2h3"/></svg>)svg"},
        {"thumbs_down", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M10 15v4a3 3 0 0 0 3 3l4-9V2H5.72a2 2 0 0 0-2 1.7l-1.38 9a2 2 0 0 0 2 2.3z"/><path d="M17 2h2.67A2.31 2.31 0 0 1 22 4v7a2.31 2.31 0 0 1-2.33 2H17"/></svg>)svg"},
        {"share", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="18" cy="5" r="3"/><circle cx="6" cy="12" r="3"/><circle cx="18" cy="19" r="3"/><line x1="8.59" y1="13.51" x2="15.42" y2="17.49"/><line x1="15.41" y1="6.51" x2="8.59" y2="10.49"/></svg>)svg"},
        {"award", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="8" r="7"/><polyline points="8.21 13.89 7 23 12 20 17 23 15.79 13.88"/></svg>)svg"},
        {"gift", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polyline points="20 12 20 22 4 22 4 12"/><rect x="2" y="7" width="20" height="5"/><line x1="12" y1="22" x2="12" y2="7"/><path d="M12 7H7.5a2.5 2.5 0 0 1 0-5C11 2 12 7 12 7z"/><path d="M12 7h4.5a2.5 2.5 0 0 0 0-5C13 2 12 7 12 7z"/></svg>)svg"},
        {"flag", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M4 15s1-1 4-1 5 2 8 2 4-1 4-1V3s-1 1-4 1-5-2-8-2-4 1-4 1z"/><line x1="4" y1="22" x2="4" y2="15"/></svg>)svg"},
        {"arrow_left", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><line x1="19" y1="12" x2="5" y2="12"/><polyline points="12 19 5 12 12 5"/></svg>)svg"},
        {"chevron_up", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polyline points="18 15 12 9 6 15"/></svg>)svg"},
        {"chevron_down", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polyline points="6 9 12 15 18 9"/></svg>)svg"},
        {"chevron_left", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polyline points="15 18 9 12 15 6"/></svg>)svg"},
        {"chevron_right", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polyline points="9 18 15 12 9 6"/></svg>)svg"},
        {"chevrons_up", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polyline points="17 11 12 6 7 11"/><polyline points="17 18 12 13 7 18"/></svg>)svg"},
        {"chevrons_down", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polyline points="7 13 12 18 17 13"/><polyline points="7 6 12 11 17 6"/></svg>)svg"},
        {"corner_down_right", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polyline points="15 10 20 15 15 20"/><path d="M4 4v7a4 4 0 0 0 4 4h12"/></svg>)svg"},
        {"corner_up_left", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polyline points="9 14 4 9 9 4"/><path d="M20 20v-7a4 4 0 0 0-4-4H4"/></svg>)svg"},
        {"check", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polyline points="20 6 9 17 4 12"/></svg>)svg"},
        {"check.svg", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="#378ADD" stroke-width="3.5" stroke-linecap="round" stroke-linejoin="round"><polyline points="20 6 9 17 4 12"/></svg>)svg"},
        {"check_circle", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M22 11.08V12a10 10 0 1 1-5.93-9.14"/><polyline points="22 4 12 14.01 9 11.01"/></svg>)svg"},
        {"x_circle", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="10"/><line x1="15" y1="9" x2="9" y2="15"/><line x1="9" y1="9" x2="15" y2="15"/></svg>)svg"},
        {"alert_circle", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="10"/><line x1="12" y1="8" x2="12" y2="12"/><line x1="12" y1="16" x2="12.01" y2="16"/></svg>)svg"},
        {"alert_triangle", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M10.29 3.86L1.82 18a2 2 0 0 0 1.71 3h16.94a2 2 0 0 0 1.71-3L13.71 3.86a2 2 0 0 0-3.42 0z"/><line x1="12" y1="9" x2="12" y2="13"/><line x1="12" y1="17" x2="12.01" y2="17"/></svg>)svg"},
        {"info", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="10"/><line x1="12" y1="16" x2="12" y2="12"/><line x1="12" y1="8" x2="12.01" y2="8"/></svg>)svg"},
        {"minus", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><line x1="5" y1="12" x2="19" y2="12"/></svg>)svg"},
        {"minus_circle", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="10"/><line x1="8" y1="12" x2="16" y2="12"/></svg>)svg"},
        {"plus_circle", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="10"/><line x1="12" y1="8" x2="12" y2="16"/><line x1="8" y1="12" x2="16" y2="12"/></svg>)svg"},
        {"phone", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M22 16.92v3a2 2 0 0 1-2.18 2 19.79 19.79 0 0 1-8.63-3.07A19.5 19.5 0 0 1 4.69 12 19.79 19.79 0 0 1 1.61 3.35 2 2 0 0 1 3.6 1h3a2 2 0 0 1 2 1.72 12.84 12.84 0 0 0 .7 2.81 2 2 0 0 1-.45 2.11L7.91 8.6a16 16 0 0 0 5.55 5.55l.92-.92a2 2 0 0 1 2.11-.45 12.84 12.84 0 0 0 2.81.7A2 2 0 0 1 21 16.92z"/></svg>)svg"},
        {"mail", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M4 4h16c1.1 0 2 .9 2 2v12c0 1.1-.9 2-2 2H4c-1.1 0-2-.9-2-2V6c0-1.1.9-2 2-2z"/><polyline points="22,6 12,13 2,6"/></svg>)svg"},
        {"inbox", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polyline points="22 12 16 12 14 15 10 15 8 12 2 12"/><path d="M5.45 5.11L2 12v6a2 2 0 0 0 2 2h16a2 2 0 0 0 2-2v-6l-3.45-6.89A2 2 0 0 0 16.76 4H7.24a2 2 0 0 0-1.79 1.11z"/></svg>)svg"},
        {"send", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><line x1="22" y1="2" x2="11" y2="13"/><polygon points="22 2 15 22 11 13 2 9 22 2"/></svg>)svg"},
        {"reply", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polyline points="9 17 4 12 9 7"/><path d="M20 18v-2a4 4 0 0 0-4-4H4"/></svg>)svg"},
        {"forward_msg", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polyline points="15 17 20 12 15 7"/><path d="M4 18v-2a4 4 0 0 1 4-4h12"/></svg>)svg"},
        {"archive", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polyline points="21 8 21 21 3 21 3 8"/><rect x="1" y="3" width="22" height="5"/><line x1="10" y1="12" x2="14" y2="12"/></svg>)svg"},
        {"at_sign", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="4"/><path d="M16 8v5a3 3 0 0 0 6 0v-1a10 10 0 1 0-3.92 7.94"/></svg>)svg"},
        {"hash", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><line x1="4" y1="9" x2="20" y2="9"/><line x1="4" y1="15" x2="20" y2="15"/><line x1="10" y1="3" x2="8" y2="21"/><line x1="16" y1="3" x2="14" y2="21"/></svg>)svg"},
        {"cloud", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M18 10h-1.26A8 8 0 1 0 9 20h9a5 5 0 0 0 0-10z"/></svg>)svg"},
        {"cloud_upload", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polyline points="16 16 12 12 8 16"/><line x1="12" y1="12" x2="12" y2="21"/><path d="M20.39 18.39A5 5 0 0 0 18 9h-1.26A8 8 0 1 0 3 16.3"/></svg>)svg"},
        {"cloud_download", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polyline points="8 17 12 21 16 17"/><line x1="12" y1="12" x2="12" y2="21"/><path d="M20.88 18.09A5 5 0 0 0 18 9h-1.26A8 8 0 1 0 3 16.29"/></svg>)svg"},
        {"wifi", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M5 12.55a11 11 0 0 1 14.08 0"/><path d="M1.42 9a16 16 0 0 1 21.16 0"/><path d="M8.53 16.11a6 6 0 0 1 6.95 0"/><line x1="12" y1="20" x2="12.01" y2="20"/></svg>)svg"},
        {"globe", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="10"/><line x1="2" y1="12" x2="22" y2="12"/><path d="M12 2a15.3 15.3 0 0 1 4 10 15.3 15.3 0 0 1-4 10 15.3 15.3 0 0 1-4-10 15.3 15.3 0 0 1 4-10z"/></svg>)svg"},
        {"server", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="2" y="2" width="20" height="8" rx="2" ry="2"/><rect x="2" y="14" width="20" height="8" rx="2" ry="2"/><line x1="6" y1="6" x2="6.01" y2="6"/><line x1="6" y1="18" x2="6.01" y2="18"/></svg>)svg"},
        {"database", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><ellipse cx="12" cy="5" rx="9" ry="3"/><path d="M21 12c0 1.66-4 3-9 3s-9-1.34-9-3"/><path d="M3 5v14c0 1.66 4 3 9 3s9-1.34 9-3V5"/></svg>)svg"},
        {"cpu", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="4" y="4" width="16" height="16" rx="2"/><rect x="9" y="9" width="6" height="6"/><line x1="9" y1="1" x2="9" y2="4"/><line x1="15" y1="1" x2="15" y2="4"/><line x1="9" y1="20" x2="9" y2="23"/><line x1="15" y1="20" x2="15" y2="23"/><line x1="20" y1="9" x2="23" y2="9"/><line x1="20" y1="14" x2="23" y2="14"/><line x1="1" y1="9" x2="4" y2="9"/><line x1="1" y1="14" x2="4" y2="14"/></svg>)svg"},
        {"hard_drive", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><line x1="22" y1="12" x2="2" y2="12"/><path d="M5.45 5.11L2 12v6a2 2 0 0 0 2 2h16a2 2 0 0 0 2-2v-6l-3.45-6.89A2 2 0 0 0 16.76 4H7.24a2 2 0 0 0-1.79 1.11z"/><line x1="6" y1="16" x2="6.01" y2="16"/><line x1="10" y1="16" x2="10.01" y2="16"/></svg>)svg"},
        {"terminal", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polyline points="4 17 10 11 4 5"/><line x1="12" y1="19" x2="20" y2="19"/></svg>)svg"},
        {"bluetooth", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polyline points="6.5 6.5 17.5 17.5 12 23 12 1 17.5 6.5 6.5 17.5"/></svg>)svg"},
        {"battery", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="1" y="6" width="18" height="12" rx="2" ry="2"/><line x1="23" y1="13" x2="23" y2="11"/></svg>)svg"},
        {"battery_charging", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M5 18H3a2 2 0 0 1-2-2V8a2 2 0 0 1 2-2h3.19M15 6h2a2 2 0 0 1 2 2v8a2 2 0 0 1-2 2h-3.19"/><line x1="23" y1="13" x2="23" y2="11"/><polyline points="11 6 7 12 13 12 9 18"/></svg>)svg"},
        {"printer", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polyline points="6 9 6 2 18 2 18 9"/><path d="M6 18H4a2 2 0 0 1-2-2v-5a2 2 0 0 1 2-2h16a2 2 0 0 1 2 2v5a2 2 0 0 1-2 2h-2"/><rect x="6" y="14" width="12" height="8"/></svg>)svg"},
        {"smartphone", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="5" y="2" width="14" height="20" rx="2" ry="2"/><line x1="12" y1="18" x2="12.01" y2="18"/></svg>)svg"},
        {"tablet", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="4" y="2" width="16" height="20" rx="2" ry="2"/><line x1="12" y1="18" x2="12.01" y2="18"/></svg>)svg"},
        {"mouse_pointer", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M3 3l7.07 16.97 2.51-7.39 7.39-2.51L3 3z"/><path d="M13 13l6 6"/></svg>)svg"},
        {"keyboard", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="2" y="6" width="20" height="12" rx="2"/><path d="M6 10h.01M10 10h.01M14 10h.01M18 10h.01M8 14h8M6 14h.01M18 14h.01"/></svg>)svg"},
        {"headphones", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M3 18v-6a9 9 0 0 1 18 0v6"/><path d="M21 19a2 2 0 0 1-2 2h-1a2 2 0 0 1-2-2v-3a2 2 0 0 1 2-2h3z"/><path d="M3 19a2 2 0 0 0 2 2h1a2 2 0 0 0 2-2v-3a2 2 0 0 0-2-2H3z"/></svg>)svg"},
        {"video", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polygon points="23 7 16 12 23 17 23 7"/><rect x="1" y="5" width="15" height="14" rx="2" ry="2"/></svg>)svg"},
        {"film", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="2" y="2" width="20" height="20" rx="2.18" ry="2.18"/><line x1="7" y1="2" x2="7" y2="22"/><line x1="17" y1="2" x2="17" y2="22"/><line x1="2" y1="12" x2="22" y2="12"/><line x1="2" y1="7" x2="7" y2="7"/><line x1="2" y1="17" x2="7" y2="17"/><line x1="17" y1="17" x2="22" y2="17"/><line x1="17" y1="7" x2="22" y2="7"/></svg>)svg"},
        {"play", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polygon points="5 3 19 12 5 21 5 3"/></svg>)svg"},
        {"pause", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="6" y="4" width="4" height="16"/><rect x="14" y="4" width="4" height="16"/></svg>)svg"},
        {"stop_circle", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="10"/><rect x="9" y="9" width="6" height="6"/></svg>)svg"},
        {"skip_forward", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polygon points="5 4 15 12 5 20 5 4"/><line x1="19" y1="5" x2="19" y2="19"/></svg>)svg"},
        {"skip_back", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polygon points="19 20 9 12 19 4 19 20"/><line x1="5" y1="19" x2="5" y2="5"/></svg>)svg"},
        {"volume", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polygon points="11 5 6 9 2 9 2 15 6 15 11 19 11 5"/><path d="M15.54 8.46a5 5 0 0 1 0 7.07"/></svg>)svg"},
        {"volume_x", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polygon points="11 5 6 9 2 9 2 15 6 15 11 19 11 5"/><line x1="23" y1="9" x2="17" y2="15"/><line x1="17" y1="9" x2="23" y2="15"/></svg>)svg"},
        {"music", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M9 18V5l12-2v13"/><circle cx="6" cy="18" r="3"/><circle cx="18" cy="16" r="3"/></svg>)svg"},
        {"mic", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M12 1a3 3 0 0 0-3 3v8a3 3 0 0 0 6 0V4a3 3 0 0 0-3-3z"/><path d="M19 10v2a7 7 0 0 1-14 0v-2"/><line x1="12" y1="19" x2="12" y2="23"/><line x1="8" y1="23" x2="16" y2="23"/></svg>)svg"},
        {"mic_off", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><line x1="1" y1="1" x2="23" y2="23"/><path d="M9 9v3a3 3 0 0 0 5.12 2.12M15 9.34V4a3 3 0 0 0-5.94-.6"/><path d="M17 16.95A7 7 0 0 1 5 12v-2m14 0v2a7 7 0 0 1-.11 1.23"/><line x1="12" y1="19" x2="12" y2="23"/><line x1="8" y1="23" x2="16" y2="23"/></svg>)svg"},
        {"shopping_cart", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="9" cy="21" r="1"/><circle cx="20" cy="21" r="1"/><path d="M1 1h4l2.68 13.39a2 2 0 0 0 2 1.61h9.72a2 2 0 0 0 2-1.61L23 6H6"/></svg>)svg"},
        {"shopping_bag", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M6 2L3 6v14a2 2 0 0 0 2 2h14a2 2 0 0 0 2-2V6l-3-4z"/><line x1="3" y1="6" x2="21" y2="6"/><path d="M16 10a4 4 0 0 1-8 0"/></svg>)svg"},
        {"credit_card", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="1" y="4" width="22" height="16" rx="2" ry="2"/><line x1="1" y1="10" x2="23" y2="10"/></svg>)svg"},
        {"dollar_sign", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><line x1="12" y1="1" x2="12" y2="23"/><path d="M17 5H9.5a3.5 3.5 0 0 0 0 7h5a3.5 3.5 0 0 1 0 7H6"/></svg>)svg"},
        {"percent", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><line x1="19" y1="5" x2="5" y2="19"/><circle cx="6.5" cy="6.5" r="2.5"/><circle cx="17.5" cy="17.5" r="2.5"/></svg>)svg"},
        {"package", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><line x1="16.5" y1="9.4" x2="7.5" y2="4.21"/><path d="M21 16V8a2 2 0 0 0-1-1.73l-7-4a2 2 0 0 0-2 0l-7 4A2 2 0 0 0 3 8v8a2 2 0 0 0 1 1.73l7 4a2 2 0 0 0 2 0l7-4A2 2 0 0 0 21 16z"/><polyline points="3.27 6.96 12 12.01 20.73 6.96"/><line x1="12" y1="22.08" x2="12" y2="12"/></svg>)svg"},
        {"truck", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="1" y="3" width="15" height="13"/><polygon points="16 8 20 8 23 11 23 16 16 16 16 8"/><circle cx="5.5" cy="18.5" r="2.5"/><circle cx="18.5" cy="18.5" r="2.5"/></svg>)svg"},
        {"bar_chart", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><line x1="18" y1="20" x2="18" y2="10"/><line x1="12" y1="20" x2="12" y2="4"/><line x1="6" y1="20" x2="6" y2="14"/><line x1="2" y1="20" x2="22" y2="20"/></svg>)svg"},
        {"pie_chart", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M21.21 15.89A10 10 0 1 1 8 2.83"/><path d="M22 12A10 10 0 0 0 12 2v10z"/></svg>)svg"},
        {"trending_up", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polyline points="23 6 13.5 15.5 8.5 10.5 1 18"/><polyline points="17 6 23 6 23 12"/></svg>)svg"},
        {"trending_down", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polyline points="23 18 13.5 8.5 8.5 13.5 1 6"/><polyline points="17 18 23 18 23 12"/></svg>)svg"},
        {"activity", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polyline points="22 12 18 12 15 21 9 3 6 12 2 12"/></svg>)svg"},
        {"sun", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="5"/><line x1="12" y1="1" x2="12" y2="3"/><line x1="12" y1="21" x2="12" y2="23"/><line x1="4.22" y1="4.22" x2="5.64" y2="5.64"/><line x1="18.36" y1="18.36" x2="19.78" y2="19.78"/><line x1="1" y1="12" x2="3" y2="12"/><line x1="21" y1="12" x2="23" y2="12"/><line x1="4.22" y1="19.78" x2="5.64" y2="18.36"/><line x1="18.36" y1="5.64" x2="19.78" y2="4.22"/></svg>)svg"},
        {"moon", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M21 12.79A9 9 0 1 1 11.21 3 7 7 0 0 0 21 12.79z"/></svg>)svg"},
        {"cloud_rain", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><line x1="16" y1="13" x2="16" y2="21"/><line x1="8" y1="13" x2="8" y2="21"/><line x1="12" y1="15" x2="12" y2="23"/><path d="M20 16.58A5 5 0 0 0 18 7h-1.26A8 8 0 1 0 4 15.25"/></svg>)svg"},
        {"wind", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M9.59 4.59A2 2 0 1 1 11 8H2m10.59 11.41A2 2 0 1 0 14 16H2m15.73-8.27A2.5 2.5 0 1 1 19.5 12H2"/></svg>)svg"},
        {"thermometer", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M14 14.76V3.5a2.5 2.5 0 0 0-5 0v11.26a4.5 4.5 0 1 0 5 0z"/></svg>)svg"},
        {"umbrella", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M23 12a11.05 11.05 0 0 0-22 0zm-5 7a3 3 0 0 1-6 0v-7"/></svg>)svg"},
        {"snowflake", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><line x1="12" y1="2" x2="12" y2="22"/><path d="M17 5H9.5a3.5 3.5 0 0 0 0 7h5a3.5 3.5 0 0 1 0 7H6"/><line x1="2" y1="12" x2="22" y2="12"/><polyline points="8 8 4 4 8 8 4 12 8 16 4 20"/><polyline points="16 8 20 4 16 8 20 12 16 16 20 20"/></svg>)svg"},
        {"git_commit", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="4"/><line x1="1.05" y1="12" x2="7" y2="12"/><line x1="17.01" y1="12" x2="22.96" y2="12"/></svg>)svg"},
        {"git_merge", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="18" cy="18" r="3"/><circle cx="6" cy="6" r="3"/><path d="M6 21V9a9 9 0 0 0 9 9"/></svg>)svg"},
        {"git_pull_request", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="18" cy="18" r="3"/><circle cx="6" cy="6" r="3"/><path d="M13 6h3a2 2 0 0 1 2 2v7"/><line x1="6" y1="9" x2="6" y2="21"/></svg>)svg"},
        {"github", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M9 19c-5 1.5-5-2.5-7-3m14 6v-3.87a3.37 3.37 0 0 0-.94-2.61c3.14-.35 6.44-1.54 6.44-7A5.44 5.44 0 0 0 20 4.77 5.07 5.07 0 0 0 19.91 1S18.73.65 16 2.48a13.38 13.38 0 0 0-7 0C6.27.65 5.09 1 5.09 1A5.07 5.07 0 0 0 5 4.77a5.44 5.44 0 0 0-1.5 3.78c0 5.42 3.3 6.61 6.44 7A3.37 3.37 0 0 0 9 18.13V22"/></svg>)svg"},
        {"underline", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M6 3v7a6 6 0 0 0 6 6 6 6 0 0 0 6-6V3"/><line x1="4" y1="21" x2="20" y2="21"/></svg>)svg"},
        {"strikethrough", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M17.3 4.9c-2.3-.6-4.4-1-6.2-.9-2.7 0-5.3.7-5.3 3.6 0 1.5 1.8 3.3 6 3.9h.2m6.7 3.7c.4.4.6.9.6 1.3 0 2.9-2.7 3.6-6.2 3.6-2.3 0-4.4-.3-6.2-.9M4 11.5h16"/></svg>)svg"},
        {"align_left", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><line x1="17" y1="10" x2="3" y2="10"/><line x1="21" y1="6" x2="3" y2="6"/><line x1="21" y1="14" x2="3" y2="14"/><line x1="17" y1="18" x2="3" y2="18"/></svg>)svg"},
        {"align_center", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><line x1="18" y1="10" x2="6" y2="10"/><line x1="21" y1="6" x2="3" y2="6"/><line x1="21" y1="14" x2="3" y2="14"/><line x1="18" y1="18" x2="6" y2="18"/></svg>)svg"},
        {"align_right", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><line x1="21" y1="10" x2="7" y2="10"/><line x1="21" y1="6" x2="3" y2="6"/><line x1="21" y1="14" x2="3" y2="14"/><line x1="21" y1="18" x2="7" y2="18"/></svg>)svg"},
        {"align_justify", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><line x1="21" y1="10" x2="3" y2="10"/><line x1="21" y1="6" x2="3" y2="6"/><line x1="21" y1="14" x2="3" y2="14"/><line x1="21" y1="18" x2="3" y2="18"/></svg>)svg"},
        {"indent", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><line x1="21" y1="6" x2="3" y2="6"/><line x1="21" y1="12" x2="9" y2="12"/><line x1="21" y1="18" x2="3" y2="18"/><polyline points="3 8 7 12 3 16"/></svg>)svg"},
        {"outdent", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><line x1="21" y1="6" x2="3" y2="6"/><line x1="15" y1="12" x2="3" y2="12"/><line x1="21" y1="18" x2="3" y2="18"/><polyline points="9 8 5 12 9 16"/></svg>)svg"},
        {"type", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polyline points="4 7 4 4 20 4 20 7"/><line x1="9" y1="20" x2="15" y2="20"/><line x1="12" y1="4" x2="12" y2="20"/></svg>)svg"},
        {"superscript", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="m4 19 8-8"/><path d="m12 19-8-8"/><path d="M20 12h-4c0-1.5.442-2 1.5-2.5S20 8.334 20 7.002c0-.472-.17-.93-.484-1.29a2.105 2.105 0 0 0-2.617-.436c-.42.239-.738.614-.899 1.06"/></svg>)svg"},
        {"map", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polygon points="1 6 1 22 8 18 16 22 23 18 23 2 16 6 8 2 1 6"/><line x1="8" y1="2" x2="8" y2="18"/><line x1="16" y1="6" x2="16" y2="22"/></svg>)svg"},
        {"compass", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="10"/><polygon points="16.24 7.76 14.12 14.12 7.76 16.24 9.88 9.88 16.24 7.76"/></svg>)svg"},
        {"navigation", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polygon points="3 11 22 2 13 21 11 13 3 11"/></svg>)svg"},
        {"anchor", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="5" r="3"/><line x1="12" y1="22" x2="12" y2="8"/><path d="M5 12H2a10 10 0 0 0 20 0h-3"/></svg>)svg"},
        {"key", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M21 2l-2 2m-7.61 7.61a5.5 5.5 0 1 1-7.778 7.778 5.5 5.5 0 0 1 7.777-7.777zm0 0L15.5 7.5m0 0l3 3L22 7l-3-3m-3.5 3.5L19 4"/></svg>)svg"},
        {"shield", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M12 22s8-4 8-10V5l-8-3-8 3v7c0 6 8 10 8 10z"/></svg>)svg"},
        {"shield_check", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M12 22s8-4 8-10V5l-8-3-8 3v7c0 6 8 10 8 10z"/><polyline points="9 12 11 14 15 10"/></svg>)svg"},
        {"eye_off", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M17.94 17.94A10.07 10.07 0 0 1 12 20c-7 0-11-8-11-8a18.45 18.45 0 0 1 5.06-5.94M9.9 4.24A9.12 9.12 0 0 1 12 4c7 0 11 8 11 8a18.5 18.5 0 0 1-2.16 3.19m-6.72-1.07a3 3 0 1 1-4.24-4.24"/><line x1="1" y1="1" x2="23" y2="23"/></svg>)svg"},
        {"unlock", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="3" y="11" width="18" height="11" rx="2"/><path d="M7 11V7a5 5 0 0 1 9.9-1"/><circle cx="7" cy="17" r="0.7" fill="currentColor"/><circle cx="12" cy="17" r="0.7" fill="currentColor"/><circle cx="17" cy="17" r="0.7" fill="currentColor"/></svg>)svg"},
        {"unlock_secure", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="3" y="11" width="18" height="11" rx="2"/><path d="M7 11V7a5 5 0 0 1 9.9-1"/><circle cx="12" cy="15.5" r="2" fill="none" stroke="currentColor" stroke-width="1.5"/><line x1="12" y1="17.5" x2="12" y2="20.5" stroke="currentColor" stroke-width="1.5" stroke-linecap="round"/></svg>)svg"},
        {"fingerprint", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M2 13.5V12a10 10 0 0 1 20 0v1.5"/><path d="M6 12a6 6 0 0 1 12 0v2"/><path d="M10 12a2 2 0 0 1 4 0v5.5"/><path d="M2 15.5A18 18 0 0 0 12 18a18 18 0 0 0 10-2.5"/></svg>)svg"},
        {"feather", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M20.24 12.24a6 6 0 0 0-8.49-8.49L5 10.5V19h8.5z"/><line x1="16" y1="8" x2="2" y2="22"/><line x1="17.5" y1="15" x2="9" y2="15"/></svg>)svg"},
        {"paperclip", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M21.44 11.05l-9.19 9.19a6 6 0 0 1-8.49-8.49l9.19-9.19a4 4 0 0 1 5.66 5.66l-9.2 9.19a2 2 0 0 1-2.83-2.83l8.49-8.48"/></svg>)svg"},
        {"scissors", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="6" cy="6" r="3"/><circle cx="6" cy="18" r="3"/><line x1="20" y1="4" x2="8.12" y2="15.88"/><line x1="14.47" y1="14.48" x2="20" y2="20"/><line x1="8.12" y1="8.12" x2="12" y2="12"/></svg>)svg"},
        {"toggle_left", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="1" y="5" width="22" height="14" rx="7" ry="7"/><circle cx="8" cy="12" r="3"/></svg>)svg"},
        {"toggle_right", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="1" y="5" width="22" height="14" rx="7" ry="7"/><circle cx="16" cy="12" r="3"/></svg>)svg"},
        {"sparkles", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="m12 3-1.912 5.813a2 2 0 0 1-1.275 1.275L3 12l5.813 1.912a2 2 0 0 1 1.275 1.275L12 21l1.912-5.813a2 2 0 0 1 1.275-1.275L21 12l-5.813-1.912a2 2 0 0 1-1.275-1.275L12 3Z"/><path d="M5 3v4"/><path d="M19 17v4"/><path d="M3 5h4"/><path d="M17 19h4"/></svg>)svg"},
        {"wand", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M15 4V2"/><path d="M15 16v-2"/><path d="M8 9h2"/><path d="M20 9h2"/><path d="M17.8 11.8 19 13"/><path d="M15 9h0"/><path d="M17.8 6.2 19 5"/><path d="m3 21 9-9"/><path d="M12.2 6.2 11 5"/></svg>)svg"},
        {"layers", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polygon points="12 2 2 7 12 12 22 7 12 2"/><polyline points="2 17 12 22 22 17"/><polyline points="2 12 12 17 22 12"/></svg>)svg"},
        {"sidebar_open", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="3" y="3" width="18" height="18" rx="2"/><path d="M9 3v18"/><path d="m16 15-3-3 3-3"/></svg>)svg"},
        {"panel_right", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="3" y="3" width="18" height="18" rx="2"/><path d="M15 3v18"/><path d="m8 9 3 3-3 3"/></svg>)svg"},
        {"fullscreen", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M8 3H5a2 2 0 0 0-2 2v3"/><path d="M21 8V5a2 2 0 0 0-2-2h-3"/><path d="M3 16v3a2 2 0 0 0 2 2h3"/><path d="M16 21h3a2 2 0 0 0 2-2v-3"/></svg>)svg"},
        {"shrink", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="m15 15 6 6m-6-6v4.8m0-4.8h4.8"/><path d="M9 19.8V15m0 0H4.2M9 15l-6 6"/><path d="M15 4.2V9m0 0h4.8M15 9l6-6"/><path d="M9 4.2V9m0 0H4.2M9 9 3 3"/></svg>)svg"},
        {"list_check", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="m3 17 2 2 4-4"/><path d="m3 7 2 2 4-4"/><line x1="13" y1="8" x2="21" y2="8"/><line x1="13" y1="16" x2="21" y2="16"/></svg>)svg"},
        {"sort_asc", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M3 6h18M7 12h10M11 18h4"/></svg>)svg"},
        {"sort_desc", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M3 18h18M7 12h10M11 6h4"/></svg>)svg"},
        {"download", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M21 15v4a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2v-4"/><polyline points="7 10 12 15 17 10"/><line x1="12" y1="15" x2="12" y2="3"/></svg>)svg"},
        {"upload", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M21 15v4a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2v-4"/><polyline points="17 8 12 3 7 8"/><line x1="12" y1="3" x2="12" y2="15"/></svg>)svg"},
        {"clock_history", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M3 12a9 9 0 1 0 9-9 9.75 9.75 0 0 0-6.74 2.74L3 8"/><path d="M3 3v5h5"/><path d="M12 7v5l4 2"/></svg>)svg"},
        {"notebook", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M2 6h4"/><path d="M2 10h4"/><path d="M2 14h4"/><path d="M2 18h4"/><rect x="4" y="2" width="16" height="20" rx="2"/><path d="M9 2v20"/></svg>)svg"},
        {"clipboard", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M16 4h2a2 2 0 0 1 2 2v14a2 2 0 0 1-2 2H6a2 2 0 0 1-2-2V6a2 2 0 0 1 2-2h2"/><rect x="8" y="2" width="8" height="4" rx="1" ry="1"/></svg>)svg"},
        {"sticky_note", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M15.5 3H5a2 2 0 0 0-2 2v14c0 1.1.9 2 2 2h14a2 2 0 0 0 2-2V8.5L15.5 3Z"/><path d="M15 3v6h6"/></svg>)svg"},
        {"rss", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M4 11a9 9 0 0 1 9 9"/><path d="M4 4a16 16 0 0 1 16 16"/><circle cx="5" cy="19" r="1"/></svg>)svg"},
        {"radio", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M4.9 19.1C1 15.2 1 8.8 4.9 4.9"/><path d="M7.8 16.2c-2.3-2.3-2.3-6.1 0-8.5"/><circle cx="12" cy="12" r="2"/><path d="M16.2 7.8c2.3 2.3 2.3 6.1 0 8.5"/><path d="M19.1 4.9C23 8.8 23 15.1 19.1 19"/></svg>)svg"},
        {"plane_landing", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M2 22h20"/><path d="M3.77 10.77 2 9l2-4.5 1.1.55a2 2 0 0 1 1.05 1.31l.5 1.06 3.33-.53A10.68 10.68 0 0 1 15 8l4 1.5a3 3 0 0 1 2 2.8v1a1 1 0 0 1-1 1l-11.5 1.63a3 3 0 0 1-2-.47z"/></svg>)svg"},
        {"helicopter", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M12 7v3"/><path d="M3 7h8l2 3h6"/><path d="M21 11v2a2 2 0 0 1-2 2H5l-2 2"/><path d="M7 17l-2 2m4-2l-2 2"/><path d="M2 7h20"/><circle cx="12" cy="7" r="1" fill="currentColor"/></svg>)svg"},
        {"sailboat", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M22 18H2a4 4 0 0 0 4 4h12a4 4 0 0 0 4-4z"/><path d="M21 14 12 2 3 14h18z"/><path d="M12 2v16"/></svg>)svg"},
        {"ship", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M2 21c.6.5 1.2 1 2.5 1 2.5 0 2.5-2 5-2s2.5 2 5 2 2.5-2 5-2c1.3 0 1.9.5 2.5 1"/><path d="M19.38 20A11.6 11.6 0 0 0 21 14l-9-4-9 4c0 2.9.94 5.34 2.81 7.76"/><path d="M19 13V7a1 1 0 0 0-1-1H6a1 1 0 0 0-1 1v6"/><path d="M12 3v4"/><path d="M11 3h2"/></svg>)svg"},
        {"train", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="4" y="3" width="16" height="13" rx="2"/><path d="M4 11h16"/><path d="M12 3v8"/><path d="m8 19-2 3"/><path d="m18 22-2-3"/><path d="M8 16h8"/><circle cx="9" cy="15.5" r="0.5" fill="currentColor"/><circle cx="15" cy="15.5" r="0.5" fill="currentColor"/></svg>)svg"},
        {"bus", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M8 6v6"/><path d="M15 6v6"/><path d="M2 12h19.6"/><path d="M18 18h3s.5-1.7.8-2.8c.1-.4.2-.8.2-1.2 0-.4-.1-.8-.2-1.2l-1.4-5C20.1 6.8 19.1 6 18 6H4a2 2 0 0 0-2 2v10h3"/><circle cx="7" cy="18" r="2"/><path d="M9 18h5"/><circle cx="16" cy="18" r="2"/></svg>)svg"},
        {"car", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M19 17H5v2H3v-4l2-6h14l2 6v4h-2v-2z"/><circle cx="7.5" cy="17.5" r="1.5"/><circle cx="16.5" cy="17.5" r="1.5"/><path d="M5 11h14"/></svg>)svg"},
        {"bicycle", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="18.5" cy="17.5" r="3.5"/><circle cx="5.5" cy="17.5" r="3.5"/><circle cx="15" cy="5" r="1"/><path d="M12 17.5V14l-3-3 4-3 2 3h2"/></svg>)svg"},
        {"taxi", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M10 2h4"/><path d="M21 8 19 2H5L3 8"/><path d="M3 8v8a2 2 0 0 0 2 2h14a2 2 0 0 0 2-2V8"/><path d="M3 8h18"/><circle cx="7.5" cy="16.5" r="1.5"/><circle cx="16.5" cy="16.5" r="1.5"/><path d="M7.5 11h9"/></svg>)svg"},
        {"cable_car", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M10 3 8 17"/><path d="M14 3l2 14"/><path d="M2 8h20"/><rect x="6" y="9" width="12" height="8" rx="1"/><path d="M9 17v4"/><path d="M15 17v4"/><path d="M9 21h6"/></svg>)svg"},
        {"hotel", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M18 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V4a2 2 0 0 0-2-2z"/><path d="M9 22V12h6v10"/><path d="M8 7h.01"/><path d="M16 7h.01"/><path d="M12 7h.01"/><path d="M12 11h.01"/><path d="M16 11h.01"/><path d="M8 11h.01"/></svg>)svg"},
        {"bed", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M2 4v16"/><path d="M2 8h18a2 2 0 0 1 2 2v10"/><path d="M2 17h20"/><path d="M6 8v9"/></svg>)svg"},
        {"tent", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M3.5 21 14 3"/><path d="M20.5 21 10 3"/><path d="M15.5 21 12 15l-3.5 6"/><path d="M2 21h20"/></svg>)svg"},
        {"cottage", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M3 22V11l9-8 9 8v11"/><path d="M3 22h18"/><path d="M9 22V16h6v6"/><path d="M6 12h.01M18 12h.01M12 8h.01"/></svg>)svg"},
        {"mountain", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="m8 3 4 8 5-5 5 15H2L8 3z"/></svg>)svg"},
        {"mountain_snow", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="m8 3 4 8 5-5 5 15H2L8 3z"/><path d="M4.14 15.08c2.62-1.57 5.24-1.43 7.86.42 2.74-1.94 5.49-2 8.23-.19"/></svg>)svg"},
        {"palm_tree", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M13 8c0-2.76-2.46-5-5.5-5S2 5.24 2 8h2l1-1 1 1h4"/><path d="M13 7.14A5.82 5.82 0 0 1 16.5 6c3.04 0 5.5 2.24 5.5 5h-3l-1-1-1 1h-3"/><path d="M5.89 9.71c-2.15 2.15-2.3 5.47-.35 7.43l4.24-4.25.7-.7.71-.71 2.12-2.12c-1.95-1.96-5.27-1.8-7.42.35z"/><path d="M11 15.5c.5 2.5-.17 4.5-1 6.5h4c2-5.5-.5-10-2-10l-1 3.5"/></svg>)svg"},
        {"castle", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M22 20v-9H2v9a2 2 0 0 0 2 2h16a2 2 0 0 0 2-2z"/><path d="M18 11V4h-4v7"/><path d="M6 11V4H2v7"/><path d="M6 7h4V4H6v3z"/><path d="M18 7h-4V4h4v3z"/><path d="M10 11V7h4v4"/><path d="M9 22v-4h6v4"/></svg>)svg"},
        {"ferris_wheel", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="2"/><path d="M12 2v4"/><path d="m6.8 4.7 2 3.5"/><path d="M3 11h4"/><path d="m4.7 17.2 3.5-2"/><path d="M12 22v-4"/><path d="m17.2 19.3-2-3.5"/><path d="M21 13h-4"/><path d="m19.3 6.8-3.5 2"/><rect x="9" y="7" width="6" height="4" rx="1"/></svg>)svg"},
        {"temple", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M2 22h20"/><path d="M5 22V6l7-4 7 4v16"/><path d="M2 6h20"/><path d="M9 22v-6h6v6"/><path d="M9 10h.01M15 10h.01M12 10h.01"/></svg>)svg"},
        {"statue", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M12 2a3 3 0 1 0 0 6 3 3 0 0 0 0-6z"/><path d="M12 8v4"/><path d="M9 12l-2 4h10l-2-4"/><path d="M8 16v6"/><path d="M16 16v6"/><path d="M5 22h14"/></svg>)svg"},
        {"museum", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M2 22h20"/><path d="M3 10h18"/><path d="M12 2 2 10h20L12 2z"/><path d="M5 10v12"/><path d="M19 10v12"/><path d="M9 10v12"/><path d="M15 10v12"/></svg>)svg"},
        {"surfing", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M17 11.5A14.5 14.5 0 0 1 2.5 3c4.5 0 8.5 2 11 5l1.5 2"/><path d="M2 15c2.5-1 5-1 7.5.5S13 18 15 17.5s4-2 5-1.5l2 1"/><path d="M2 19c2.5-1 5-1 7.5.5S13 22 15 21.5s4-2 5-1.5l2 1"/></svg>)svg"},
        {"swimming", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M2 6c.6.5 1.2 1 2.5 1C7 7 7 5 9.5 5s2.5 2 5 2 2.5-2 5-2c1.3 0 1.9.5 2.5 1"/><path d="M2 12c.6.5 1.2 1 2.5 1 2.5 0 2.5-2 5-2s2.5 2 5 2 2.5-2 5-2c1.3 0 1.9.5 2.5 1"/><path d="M2 18c.6.5 1.2 1 2.5 1 2.5 0 2.5-2 5-2s2.5 2 5 2 2.5-2 5-2c1.3 0 1.9.5 2.5 1"/></svg>)svg"},
        {"binoculars", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="6" cy="15" r="4"/><circle cx="18" cy="15" r="4"/><path d="M10 15a4 4 0 0 1 4 0"/><path d="M6 5V3"/><path d="M18 5V3"/><path d="M6 5h12"/></svg>)svg"},
        {"ticket", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M2 9a3 3 0 0 1 0 6v2a2 2 0 0 0 2 2h16a2 2 0 0 0 2-2v-2a3 3 0 0 1 0-6V7a2 2 0 0 0-2-2H4a2 2 0 0 0-2 2z"/><line x1="9" y1="9" x2="9" y2="15" stroke-dasharray="2 2"/></svg>)svg"},
        {"backpack", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M4 10a4 4 0 0 1 4-4h8a4 4 0 0 1 4 4v10a2 2 0 0 1-2 2H6a2 2 0 0 1-2-2z"/><path d="M9 6V4a3 3 0 0 1 6 0v2"/><path d="M8 21v-5a2 2 0 0 1 2-2h4a2 2 0 0 1 2 2v5"/><path d="M8 10h8"/><path d="M8 18h8"/></svg>)svg"},
        {"luggage", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M6 20a2 2 0 0 1-2-2V8a2 2 0 0 1 2-2h12a2 2 0 0 1 2 2v10a2 2 0 0 1-2 2z"/><path d="M8 6V4a2 2 0 0 1 2-2h4a2 2 0 0 1 2 2v2"/><line x1="12" y1="12" x2="12.01" y2="12"/><path d="M8 13h8"/><circle cx="7" cy="21" r="1"/><circle cx="17" cy="21" r="1"/></svg>)svg"},
        {"map_pin", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M20 10c0 6-8 12-8 12S4 16 4 10a8 8 0 1 1 16 0z"/><circle cx="12" cy="10" r="3"/></svg>)svg"},
        {"waypoints", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="4.5" r="2.5"/><path d="m10.2 6.3-3.9 3.9"/><circle cx="4.5" cy="12" r="2.5"/><path d="M7 12h10"/><circle cx="19.5" cy="12" r="2.5"/><path d="m13.8 17.7 3.9-3.9"/><circle cx="12" cy="19.5" r="2.5"/></svg>)svg"},
        {"route", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="6" cy="19" r="3"/><path d="M9 19h8.5a3.5 3.5 0 0 0 0-7h-11a3.5 3.5 0 0 1 0-7H15"/><circle cx="18" cy="5" r="3"/></svg>)svg"},
        {"road", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M8 22l4-8 4 8"/><path d="M12 14V2"/><path d="M2 22h2.5l3-8h9l3 8H22"/><path d="M12 8H4"/><path d="M12 8h8"/></svg>)svg"},
        {"utensils", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M3 2v7c0 1.1.9 2 2 2h4a2 2 0 0 0 2-2V2"/><path d="M7 2v20"/><path d="M21 15V2v0a5 5 0 0 0-5 5v6c0 1.1.9 2 2 2h3zm0 0v7"/></svg>)svg"},
        {"wine", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M8 22h8"/><path d="M7 10h10"/><path d="M12 15v7"/><path d="M12 15a5 5 0 0 0 5-5c0-2-.5-4-2-8H9c-1.5 4-2 6-2 8a5 5 0 0 0 5 5z"/></svg>)svg"},
        {"beer", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M17 11h1a3 3 0 0 1 0 6h-1"/><path d="M9 12v6"/><path d="M13 12v6"/><path d="M14 7.5c-1 0-1.44.5-3 .5s-2-.5-3-.5-1.72.5-2.5.5a2.5 2.5 0 0 1 0-5c.78 0 1.57.5 2.5.5S9.44 3 11 3s2 .5 3 .5 1.72-.5 2.5-.5a2.5 2.5 0 0 1 0 5c-.78 0-1.5-.5-2.5-.5z"/><path d="M5 7.5V20a2 2 0 0 0 2 2h8a2 2 0 0 0 2-2V7.5"/></svg>)svg"},
        {"ice_cream", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="m7 11 4.08 10.35a1 1 0 0 0 1.84 0L17 11"/><path d="M17 7A5 5 0 0 0 7 7"/><path d="M11 3a3 3 0 0 0 0 6h2a3 3 0 0 0 0-6h-2Z"/></svg>)svg"},
        {"cake", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M20 21v-8a2 2 0 0 0-2-2H6a2 2 0 0 0-2 2v8"/><path d="M4 16s.5-1 2-1 2.5 2 4 2 2.5-2 4-2 2.5 2 4 2 2-1 2-1"/><path d="M2 21h20"/><path d="M7 8v3"/><path d="M12 8v3"/><path d="M17 8v3"/><path d="M7 4 8.5 3 10 4l1.5-1L13 4l1.5-1 1.5 1"/></svg>)svg"},
        {"coffee_shop", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M17 8h1a4 4 0 1 1 0 8h-1"/><path d="M3 8h14v9a4 4 0 0 1-4 4H7a4 4 0 0 1-4-4z"/><path d="M6 2v2"/><path d="M10 2v2"/><path d="M14 2v2"/></svg>)svg"},
        {"waves", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M2 6c.6.5 1.2 1 2.5 1C7 7 7 5 9.5 5s2.5 2 5 2 2.5-2 5-2c1.3 0 1.9.5 2.5 1"/><path d="M2 12c.6.5 1.2 1 2.5 1 2.5 0 2.5-2 5-2s2.5 2 5 2 2.5-2 5-2c1.3 0 1.9.5 2.5 1"/><path d="M2 18c.6.5 1.2 1 2.5 1 2.5 0 2.5-2 5-2s2.5 2 5 2 2.5-2 5-2c1.3 0 1.9.5 2.5 1"/></svg>)svg"},
        {"forest", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="9" y="12" width="6" height="10"/><path d="M9 22H6l3-7H4l8-13 8 13h-5l3 7h-3"/></svg>)svg"},
        {"flower", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="3"/><path d="M12 2a3 3 0 0 0-3 3 3 3 0 0 0 3 3"/><path d="M22 12a3 3 0 0 0-3-3 3 3 0 0 0-3 3"/><path d="M12 22a3 3 0 0 0 3-3 3 3 0 0 0-3-3"/><path d="M2 12a3 3 0 0 0 3 3 3 3 0 0 0 3-3"/><path d="M6.34 6.34a3 3 0 0 0 0 4.24 3 3 0 0 0 4.24 0"/><path d="M17.66 6.34a3 3 0 0 0-4.24 0 3 3 0 0 0 0 4.24"/><path d="M17.66 17.66a3 3 0 0 0 0-4.24 3 3 0 0 0-4.24 0"/><path d="M6.34 17.66a3 3 0 0 0 4.24 0 3 3 0 0 0 0-4.24"/></svg>)svg"},
        {"campfire", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M12 22a5 5 0 0 0 5-5c0-2-1-3.9-3-5.5s-3.5-4-4-6.5c-.5 2.5-2 4.9-4 6.5C4 13.1 3 15 3 17a5 5 0 0 0 5 5"/><path d="M12 22V12"/></svg>)svg"},
        {"globe_2", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M21.54 15H17a2 2 0 0 0-2 2v4.54"/><path d="M7 3.34V5a3 3 0 0 0 3 3h0a2 2 0 0 1 2 2 2 2 0 0 0 4 0 2 2 0 0 1 2-2h3.17"/><path d="M11 21.95V18a2 2 0 0 0-2-2h0a2 2 0 0 1-2-2v-1a2 2 0 0 0-2-2H2.05"/><circle cx="12" cy="12" r="10"/></svg>)svg"},
        {"earth", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="10"/><path d="M12 2a14.5 14.5 0 0 0 0 20 14.5 14.5 0 0 0 0-20"/><path d="M2 12h20"/></svg>)svg"},
        {"currency_exchange", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M21 12V7H5a2 2 0 0 1 0-4h14v4"/><path d="M3 5v14a2 2 0 0 0 2 2h16v-5"/><path d="M18 12a2 2 0 0 0 0 4h4v-4z"/></svg>)svg"},
        {"suitcase", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="2" y="7" width="20" height="14" rx="2"/><path d="M16 7V5a2 2 0 0 0-2-2h-4a2 2 0 0 0-2 2v2"/><line x1="12" y1="12" x2="12" y2="16"/><line x1="10" y1="14" x2="14" y2="14"/></svg>)svg"},
        {"boarding_pass", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="2" y="5" width="20" height="14" rx="2"/><line x1="2" y1="12" x2="22" y2="12" stroke-dasharray="4 2"/><path d="M7 9h1M7 15h1M11 9h6M11 15h2"/></svg>)svg"},
        {"photo_camera", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M14.5 4h-5L7 7H4a2 2 0 0 0-2 2v9a2 2 0 0 0 2 2h16a2 2 0 0 0 2-2V9a2 2 0 0 0-2-2h-3z"/><circle cx="12" cy="13" r="3"/></svg>)svg"},
        {"expand_fullscreen", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools --> <svg fill="currentColor" width="800px" height="800px" viewBox="0 0 1920 1920" xmlns="http://www.w3.org/2000/svg"> <path d="M1146.616-.012V232.38h376.821L232.391 1523.309v-376.705H0V1920h773.629v-232.39H396.69L1687.737 396.68V773.5h232.275V-.011z" fill-rule="evenodd"/> </svg>)svg"},
        {"standby", R"svg(<?xml version="1.0" encoding="utf-8"?> <!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools --> <svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg"> <title>standby</title> <path d="M2.016 18.016q0 2.848 1.088 5.44t2.976 4.448 4.48 3.008 5.44 1.088 5.44-1.088 4.48-3.008 2.976-4.448 1.12-5.44q0-4.128-2.208-7.488t-5.792-5.088v4.608q1.856 1.408 2.912 3.488t1.088 4.48q0 2.72-1.344 5.024t-3.648 3.616-5.024 1.344q-2.016 0-3.872-0.8t-3.2-2.112-2.144-3.2-0.768-3.872q0-2.4 1.056-4.48t2.944-3.488v-4.608q-3.616 1.728-5.824 5.088t-2.176 7.488zM14.016 14.016q0 0.832 0.576 1.408t1.408 0.576 1.408-0.576 0.608-1.408v-12q0-0.832-0.608-1.408t-1.408-0.608-1.408 0.608-0.576 1.408v12z"></path> </svg>)svg"},
        {"adventure", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="m3 17 4-8 4 4 4-9 4 13"/><path d="M3 21h18"/></svg>)svg"},
        {"pier", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M4 22V2"/><path d="M20 22V2"/><path d="M4 12h16"/><path d="M4 7h16"/><path d="M2 22h20"/><path d="M8 22v-5"/><path d="M12 22v-5"/><path d="M16 22v-5"/></svg>)svg"},
        {"sunrise", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M12 2v4"/><path d="m4.93 10.93 1.41 1.41"/><path d="M2 18h2"/><path d="M20 18h2"/><path d="m19.07 10.93-1.41 1.41"/><path d="M22 22H2"/><path d="m16 6-4 4-4-4"/><circle cx="12" cy="13" r="4" stroke-dasharray="3 1"/></svg>)svg"},
        {"gondola", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M2 20h20"/><path d="M6 20V10l6-8 6 8v10"/><path d="M9 20v-5h6v5"/><path d="M3 10c0 0 3-2 9-2s9 2 9 2"/></svg>)svg"},
        {"airport", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M3 22h18"/><path d="M12 2v8"/><path d="M4.6 6.6 12 10l7.4-3.4"/><path d="M12 10v12"/><path d="M7 14h10"/></svg>)svg"},
        {"signpost", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M12 3v3"/><path d="M18.5 13H5.5L3 10.5 5.5 8h13l2.5 2.5L18.5 13z"/><path d="M12 13v8"/></svg>)svg"},
        {"double_signpost", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M12 3v3"/><path d="M18.5 10H5.5l-3-2.5L5.5 5h13"/><path d="M5.5 14h13l3 2.5-3 2.5H5.5"/><path d="M12 14v7"/></svg>)svg"},
        {"file_word", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M14 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V8z"></path><polyline points="14 2 14 8 20 8"></polyline><path d="M9 12h6"></path><path d="M9 16h6"></path><path d="M12 8V2"></path></svg>)svg"},
        {"file_ppt", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M14 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V8z"></path><polyline points="14 2 14 8 20 8"></polyline><rect x="8" y="11" width="8" height="6" rx="1"></rect><line x1="12" y1="11" x2="12" y2="17"></line></svg>)svg"},
        {"file_executable", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M14 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V8z"></path><polyline points="14 2 14 8 20 8"></polyline><path d="m9 15 3-3 3 3-3 3-3-3Z"></path></svg>)svg"},
        {"sidebar_filled", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M3 5a2 2 0 0 1 2-2h4v18H5a2 2 0 0 1-2-2z" fill="currentColor" stroke="none"/><rect x="3" y="3" width="18" height="18" rx="2"/><line x1="9" y1="3" x2="9" y2="21"/></svg>)svg"},
        {"sidebar_right_filled", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M15 3h4a2 2 0 0 1 2 2v14a2 2 0 0 1-2 2h-4z" fill="currentColor" stroke="none"/><rect x="3" y="3" width="18" height="18" rx="2"/><line x1="15" y1="3" x2="15" y2="21"/></svg>)svg"},
        {"sidebar_open_filled", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M3 5a2 2 0 0 1 2-2h4v18H5a2 2 0 0 1-2-2z" fill="currentColor" stroke="none"/><rect x="3" y="3" width="18" height="18" rx="2"/><line x1="9" y1="3" x2="9" y2="21"/><polyline points="16 15 13 12 16 9"/></svg>)svg"},
        {"columns_filled", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M3 5a2 2 0 0 1 2-2h7v18H5a2 2 0 0 1-2-2z" fill="currentColor" stroke="none"/><rect x="3" y="3" width="18" height="18" rx="2"/><line x1="12" y1="3" x2="12" y2="21"/></svg>)svg"},
        {"panel_right_filled", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M15 3h4a2 2 0 0 1 2 2v14a2 2 0 0 1-2 2h-4z" fill="currentColor" stroke="none"/><rect x="3" y="3" width="18" height="18" rx="2"/><line x1="15" y1="3" x2="15" y2="21"/><polyline points="8 9 11 12 8 15"/></svg>)svg"},
        {"thumbs_up_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><path d="M14 9V5a3 3 0 0 0-3-3l-4 9v11h11.28a2 2 0 0 0 2-1.7l1.38-9a2 2 0 0 0-2-2.3z"/><path d="M7 22H4a2 2 0 0 1-2-2v-7a2 2 0 0 1 2-2h3z"/></svg>)svg"},
        {"thumbs_down_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><path d="M10 15v4a3 3 0 0 0 3 3l4-9V2H5.72a2 2 0 0 0-2 1.7l-1.38 9a2 2 0 0 0 2 2.3z"/><path d="M17 2h2.67A2.31 2.31 0 0 1 22 4v7a2.31 2.31 0 0 1-2.33 2H17z"/></svg>)svg"},
        {"home_screen", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="5" y="2" width="14" height="20" rx="2"/><line x1="5" y1="17" x2="19" y2="17"/><circle cx="12" cy="19.5" r="0.8" fill="currentColor" stroke="none"/><rect x="7.5" y="5" width="3.5" height="3.5" rx="0.8"/><rect x="13" y="5" width="3.5" height="3.5" rx="0.8"/><rect x="7.5" y="10" width="3.5" height="3.5" rx="0.8"/><rect x="13" y="10" width="3.5" height="3.5" rx="0.8"/></svg>)svg"},
        {"alert_circle_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><circle cx="12" cy="12" r="10"/><line x1="12" y1="8" x2="12" y2="12" stroke="white" stroke-width="2" stroke-linecap="round"/><circle cx="12" cy="16" r="1" fill="white"/></svg>)svg"},
        {"alert_triangle_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><path d="M10.29 3.86L1.82 18a2 2 0 0 0 1.71 3h16.94a2 2 0 0 0 1.71-3L13.71 3.86a2 2 0 0 0-3.42 0z"/><line x1="12" y1="9" x2="12" y2="13" stroke="white" stroke-width="2" stroke-linecap="round"/><circle cx="12" cy="17" r="1" fill="white"/></svg>)svg"},
        {"award_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><circle cx="12" cy="8" r="7"/><circle cx="12" cy="8" r="4" fill="white"/><polyline points="8.21 13.89 7 23 12 20 17 23 15.79 13.88" fill="currentColor"/></svg>)svg"},
        {"bell_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><path d="M12 22a2 2 0 0 0 2-2h-4a2 2 0 0 0 2 2z"/><path d="M18.364 5.636A9 9 0 0 0 5.636 18.364L4 20h16l-1.636-1.636A9 9 0 0 0 18.364 5.636z"/><path d="M12 2a7 7 0 0 1 7 7v4.17l1.7 1.7A1 1 0 0 1 20 17H4a1 1 0 0 1-.7-1.71L5 13.17V9a7 7 0 0 1 7-7z"/></svg>)svg"},
        {"calendar_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><rect x="3" y="4" width="18" height="18" rx="2"/><path d="M3 10h18" fill="none" stroke="white" stroke-width="1.5"/><path d="M8 2v4M16 2v4" fill="none" stroke="white" stroke-width="1.5"/></svg>)svg"},
        {"camera_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><path d="M23 19a2 2 0 0 1-2 2H3a2 2 0 0 1-2-2V8a2 2 0 0 1 2-2h4l2-3h6l2 3h4a2 2 0 0 1 2 2z"/><circle cx="12" cy="13" r="4" fill="white"/></svg>)svg"},
        {"check_circle_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><circle cx="12" cy="12" r="10"/><polyline points="9 12 11 14 15 10" fill="none" stroke="white" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"/></svg>)svg"},
        {"clock_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><circle cx="12" cy="12" r="10"/><polyline points="12 6 12 12 16 14" fill="none" stroke="white" stroke-width="1.5" stroke-linecap="round"/></svg>)svg"},
        {"cloud_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><path d="M18 10h-1.26A8 8 0 1 0 9 20h9a5 5 0 0 0 0-10z"/></svg>)svg"},
        {"compass_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><circle cx="12" cy="12" r="10"/><polygon points="16.24 7.76 14.12 14.12 7.76 16.24 9.88 9.88 16.24 7.76" fill="white"/></svg>)svg"},
        {"copy_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><rect x="9" y="9" width="13" height="13" rx="2"/><path d="M5 15H4a2 2 0 0 1-2-2V4a2 2 0 0 1 2-2h9a2 2 0 0 1 2 2v1" fill="none" stroke="currentColor" stroke-width="2"/></svg>)svg"},
        {"eye_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><path d="M1 12s4-8 11-8 11 8 11 8-4 8-11 8-11-8-11-8z"/><circle cx="12" cy="12" r="3" fill="white"/></svg>)svg"},
        {"file_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><path d="M14 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V8z"/><polyline points="14 2 14 8 20 8" fill="none" stroke="white" stroke-width="1.5"/></svg>)svg"},
        {"flag_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><path d="M5 3a1 1 0 0 0-1 1v16a1 1 0 1 0 2 0v-6h13l-3-4 3-4H6V4a1 1 0 0 0-1-1z"/></svg>)svg"},
        {"folder_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><path d="M22 19a2 2 0 0 1-2 2H4a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h5l2 3h9a2 2 0 0 1 2 2z"/></svg>)svg"},
        {"gift_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><rect x="2" y="7" width="20" height="4" rx="1"/><rect x="3" y="11" width="18" height="10" rx="1"/><path d="M12 7H7.5a2.5 2.5 0 0 1 0-5C10 2 12 7 12 7z"/><path d="M12 7h4.5a2.5 2.5 0 0 0 0-5C14 2 12 7 12 7z"/><line x1="12" y1="7" x2="12" y2="21" stroke="white" stroke-width="1.5"/><line x1="2" y1="11" x2="22" y2="11" stroke="white" stroke-width="1.5"/></svg>)svg"},
        {"globe_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><circle cx="12" cy="12" r="10"/><path d="M2 12h20M12 2a15.3 15.3 0 0 1 4 10 15.3 15.3 0 0 1-4 10 15.3 15.3 0 0 1-4-10 15.3 15.3 0 0 1 4-10z" fill="none" stroke="white" stroke-width="1.3"/></svg>)svg"},
        {"grid_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><rect x="3" y="3" width="7" height="7" rx="1"/><rect x="14" y="3" width="7" height="7" rx="1"/><rect x="3" y="14" width="7" height="7" rx="1"/><rect x="14" y="14" width="7" height="7" rx="1"/></svg>)svg"},
        {"help_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><circle cx="12" cy="12" r="10"/><path d="M9.09 9a3 3 0 0 1 5.83 1c0 2-3 3-3 3" fill="none" stroke="white" stroke-width="1.8" stroke-linecap="round"/><circle cx="12" cy="17" r="1" fill="white"/></svg>)svg"},
        {"home_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><path d="M3 9l9-7 9 7v11a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2z"/><rect x="9" y="12" width="6" height="9" fill="white"/></svg>)svg"},
        {"image_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><rect x="3" y="3" width="18" height="18" rx="2"/><circle cx="8.5" cy="8.5" r="1.5" fill="white"/><polyline points="21 15 16 10 5 21" fill="white"/></svg>)svg"},
        {"inbox_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><path d="M22 12h-6l-2 3H10l-2-3H2"/><path d="M5.45 5.11L2 12v6a2 2 0 0 0 2 2h16a2 2 0 0 0 2-2v-6l-3.45-6.89A2 2 0 0 0 17.24 4H6.76a2 2 0 0 0-1.79 1.11z"/></svg>)svg"},
        {"info_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><circle cx="12" cy="12" r="10"/><line x1="12" y1="16" x2="12" y2="12" stroke="white" stroke-width="2" stroke-linecap="round"/><circle cx="12" cy="8" r="1" fill="white"/></svg>)svg"},
        {"layout_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><rect x="3" y="3" width="18" height="7" rx="2"/><rect x="3" y="13" width="8" height="8" rx="2"/><rect x="14" y="13" width="7" height="8" rx="2"/></svg>)svg"},
        {"location_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><path d="M12 2C8.13 2 5 5.13 5 9c0 5.25 7 13 7 13s7-7.75 7-13c0-3.87-3.13-7-7-7z"/><circle cx="12" cy="9" r="2.5" fill="white"/></svg>)svg"},
        {"lock_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><rect x="3" y="11" width="18" height="11" rx="2"/><path d="M7 11V7a5 5 0 0 1 10 0v4" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round"/><circle cx="7" cy="17" r="0.7" fill="white"/><circle cx="12" cy="17" r="0.7" fill="white"/><circle cx="17" cy="17" r="0.7" fill="white"/></svg>)svg"},
        {"mail_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><path d="M20 4H4a2 2 0 0 0-2 2v12a2 2 0 0 0 2 2h16a2 2 0 0 0 2-2V6a2 2 0 0 0-2-2z"/><polyline points="22 6 12 13 2 6" fill="none" stroke="white" stroke-width="1.5"/></svg>)svg"},
        {"map_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><polygon points="1 6 1 22 8 18 16 22 23 18 23 2 16 6 8 2 1 6"/><line x1="8" y1="2" x2="8" y2="18" stroke="white" stroke-width="1.5"/><line x1="16" y1="6" x2="16" y2="22" stroke="white" stroke-width="1.5"/></svg>)svg"},
        {"map_pin_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><path d="M21 10c0 7-9 13-9 13S3 17 3 10a9 9 0 0 1 18 0z"/><circle cx="12" cy="10" r="3" fill="white"/></svg>)svg"},
        {"message_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><path d="M21 15a2 2 0 0 1-2 2H7l-4 4V5a2 2 0 0 1 2-2h14a2 2 0 0 1 2 2z"/></svg>)svg"},
        {"mic_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><rect x="9" y="2" width="6" height="11" rx="3"/><path d="M19 10v2a7 7 0 0 1-14 0v-2" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round"/><line x1="12" y1="19" x2="12" y2="23" stroke="currentColor" stroke-width="2" stroke-linecap="round"/><line x1="8" y1="23" x2="16" y2="23" stroke="currentColor" stroke-width="2" stroke-linecap="round"/></svg>)svg"},
        {"minus_circle_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><circle cx="12" cy="12" r="10"/><line x1="8" y1="12" x2="16" y2="12" stroke="white" stroke-width="2" stroke-linecap="round"/></svg>)svg"},
        {"moon_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><path d="M21 12.79A9 9 0 1 1 11.21 3 7 7 0 0 0 21 12.79z"/></svg>)svg"},
        {"music_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><path d="M9 18V5l12-2v13"/><circle cx="6" cy="18" r="3"/><circle cx="18" cy="16" r="3"/></svg>)svg"},
        {"navigation_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><polygon points="3 11 22 2 13 21 11 13 3 11"/></svg>)svg"},
        {"pause_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><circle cx="12" cy="12" r="10"/><line x1="10" y1="8" x2="10" y2="16" stroke="white" stroke-width="2" stroke-linecap="round"/><line x1="14" y1="8" x2="14" y2="16" stroke="white" stroke-width="2" stroke-linecap="round"/></svg>)svg"},
        {"phone_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><path d="M22 16.92v3a2 2 0 0 1-2.18 2 19.79 19.79 0 0 1-8.63-3.07 19.5 19.5 0 0 1-6-6 19.79 19.79 0 0 1-3.07-8.67A2 2 0 0 1 4.11 2h3a2 2 0 0 1 2 1.72c.127.96.361 1.903.7 2.81a2 2 0 0 1-.45 2.11L8.09 9.91a16 16 0 0 0 6 6l1.27-1.27a2 2 0 0 1 2.11-.45c.907.339 1.85.573 2.81.7A2 2 0 0 1 22 16.92z"/></svg>)svg"},
        {"play_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><circle cx="12" cy="12" r="10"/><polygon points="10 8 16 12 10 16" fill="white"/></svg>)svg"},
        {"plus_circle_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><circle cx="12" cy="12" r="10"/><line x1="12" y1="8" x2="12" y2="16" stroke="white" stroke-width="2" stroke-linecap="round"/><line x1="8" y1="12" x2="16" y2="12" stroke="white" stroke-width="2" stroke-linecap="round"/></svg>)svg"},
        {"save_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><path d="M19 21H5a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h11l5 5v11a2 2 0 0 1-2 2z"/><rect x="8" y="14" width="8" height="7" fill="white"/><rect x="9" y="3" width="6" height="5" fill="white"/></svg>)svg"},
        {"search_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><circle cx="11" cy="11" r="8"/><circle cx="11" cy="11" r="4.5" fill="white"/><line x1="21" y1="21" x2="16.65" y2="16.65" stroke="currentColor" stroke-width="2.5" stroke-linecap="round"/></svg>)svg"},
        {"send_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><path d="M22 2L11 13"/><path d="M22 2L15 22l-4-9-9-4 20-7z"/></svg>)svg"},
        {"settings_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><path d="M12 15a3 3 0 1 0 0-6 3 3 0 0 0 0 6z"/><path d="M19.4 15a1.65 1.65 0 0 0 .33 1.82l.06.06a2 2 0 0 1-2.83 2.83l-.06-.06a1.65 1.65 0 0 0-1.82-.33 1.65 1.65 0 0 0-1 1.51V21a2 2 0 0 1-4 0v-.09A1.65 1.65 0 0 0 9 19.4a1.65 1.65 0 0 0-1.82.33l-.06.06a2 2 0 0 1-2.83-2.83l.06-.06A1.65 1.65 0 0 0 4.68 15a1.65 1.65 0 0 0-1.51-1H3a2 2 0 0 1 0-4h.09A1.65 1.65 0 0 0 4.6 9a1.65 1.65 0 0 0-.33-1.82l-.06-.06a2 2 0 0 1 2.83-2.83l.06.06A1.65 1.65 0 0 0 9 4.68a1.65 1.65 0 0 0 1-1.51V3a2 2 0 0 1 4 0v.09a1.65 1.65 0 0 0 1 1.51 1.65 1.65 0 0 0 1.82-.33l.06-.06a2 2 0 0 1 2.83 2.83l-.06.06A1.65 1.65 0 0 0 19.4 9a1.65 1.65 0 0 0 1.51 1H21a2 2 0 0 1 0 4h-.09a1.65 1.65 0 0 0-1.51 1z"/></svg>)svg"},
        {"share_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><circle cx="18" cy="5" r="3"/><circle cx="6" cy="12" r="3"/><circle cx="18" cy="19" r="3"/><path d="M8.59 13.51l6.83 3.98M15.41 6.51l-6.82 3.98" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round"/></svg>)svg"},
        {"shield_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><path d="M12 22s8-4 8-10V5l-8-3-8 3v7c0 6 8 10 8 10z"/></svg>)svg"},
        {"snowflake_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><line x1="12" y1="2" x2="12" y2="22" stroke="currentColor" stroke-width="2.5" stroke-linecap="round"/><line x1="2" y1="12" x2="22" y2="12" stroke="currentColor" stroke-width="2.5" stroke-linecap="round"/><line x1="4.93" y1="4.93" x2="19.07" y2="19.07" stroke="currentColor" stroke-width="2.5" stroke-linecap="round"/><line x1="19.07" y1="4.93" x2="4.93" y2="19.07" stroke="currentColor" stroke-width="2.5" stroke-linecap="round"/><circle cx="12" cy="12" r="2.5"/><circle cx="12" cy="2" r="1.5"/><circle cx="12" cy="22" r="1.5"/><circle cx="2" cy="12" r="1.5"/><circle cx="22" cy="12" r="1.5"/></svg>)svg"},
        {"star", R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16" fill="currentColor"><path d="M8 .25a.75.75 0 0 1 .673.418l1.882 3.815 4.21.612a.75.75 0 0 1 .416 1.279l-3.046 2.97.719 4.192a.751.751 0 0 1-1.088.791L8 12.347l-3.766 1.98a.75.75 0 0 1-1.088-.79l.72-4.194L.818 6.374a.75.75 0 0 1 .416-1.28l4.21-.611L7.327.668A.75.75 0 0 1 8 .25Zm0 2.445L6.615 5.5a.75.75 0 0 1-.564.41l-3.097.45 2.24 2.184a.75.75 0 0 1 .216.664l-.528 3.084 2.769-1.456a.75.75 0 0 1 .698 0l2.77 1.456-.53-3.084a.75.75 0 0 1 .216-.664l2.24-2.183-3.096-.45a.75.75 0 0 1-.564-.41L8 2.694Z"/></svg>)svg"},
        {"star_filled", R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16" fill="currentColor"><path d="M8 .25a.75.75 0 0 1 .673.418l1.882 3.815 4.21.612a.75.75 0 0 1 .416 1.279l-3.046 2.97.719 4.192a.751.751 0 0 1-1.088.791L8 12.347l-3.766 1.98a.75.75 0 0 1-1.088-.79l.72-4.194L.818 6.374a.75.75 0 0 1 .416-1.28l4.21-.611L7.327.668A.75.75 0 0 1 8 .25Z"/></svg>)svg"},
        {"star-001.svg", R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16" fill="currentColor"><path d="M8 .25a.75.75 0 0 1 .673.418l1.882 3.815 4.21.612a.75.75 0 0 1 .416 1.279l-3.046 2.97.719 4.192a.751.751 0 0 1-1.088.791L8 12.347l-3.766 1.98a.75.75 0 0 1-1.088-.79l.72-4.194L.818 6.374a.75.75 0 0 1 .416-1.28l4.21-.611L7.327.668A.75.75 0 0 1 8 .25Z"/></svg>)svg"},
        {"star-002.svg", R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16" fill="currentColor"><path d="M8 .25a.75.75 0 0 1 .673.418l1.882 3.815 4.21.612a.75.75 0 0 1 .416 1.279l-3.046 2.97.719 4.192a.751.751 0 0 1-1.088.791L8 12.347l-3.766 1.98a.75.75 0 0 1-1.088-.79l.72-4.194L.818 6.374a.75.75 0 0 1 .416-1.28l4.21-.611L7.327.668A.75.75 0 0 1 8 .25Zm0 2.445L6.615 5.5a.75.75 0 0 1-.564.41l-3.097.45 2.24 2.184a.75.75 0 0 1 .216.664l-.528 3.084 2.769-1.456a.75.75 0 0 1 .698 0l2.77 1.456-.53-3.084a.75.75 0 0 1 .216-.664l2.24-2.183-3.096-.45a.75.75 0 0 1-.564-.41L8 2.694Z"/></svg>)svg"},
        {"scroll-003.svg", R"svg(<svg version="1.1" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 648 648" fill="currentColor"><polygon points="162.5,613.5 162.5,42.5 486.5,328 "/></svg>)svg"},
        {"scroll-004.svg", R"svg(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 648 648" fill="currentColor"><path d="M610 504H39l285.5-324z"/></svg>)svg"},
        {"scroll-005.svg", R"svg(<svg version="1.1" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 648 648" fill="currentColor"><polygon points="486.5,42.5 486.5,613.5 162.5,328 "/></svg>)svg"},
        {"scroll-006.svg", R"svg(<svg version="1.1" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 648 648" fill="currentColor"><polygon points="28,180 619,180 323.5,504 "/></svg>)svg"},
        {"scroll-007.svg", R"svg(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 60 60" fill="currentColor"><path d="M56.5 46.7H3.6l26.4-30z"/></svg>)svg"},
        {"scroll-008.svg", R"svg(<svg version="1.1" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 648 648" fill="currentColor"><polygon points="162.5,613.5 162.5,42.5 486.5,328 "/></svg>)svg"},
        {"scroll-009.svg", R"svg(<svg version="1.1" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 648 648" fill="currentColor"><polygon points="486.5,42.5 486.5,613.5 162.5,328 "/></svg>)svg"},
        {"scroll-010.svg", R"svg(<svg version="1.1" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 60 60" fill="currentColor"><polygon points="3.6,16.7 56.5,16.7 30,46.7 "/></svg>)svg"},
        {"star-rate-rating-outline-svgrepo-com.svg", R"svg(<svg viewBox="0 0 64 64" fill="currentColor"><path d="M37.675,26.643l18.335,0l-14.834,10.777l5.666,17.438l-14.833,-10.777l-14.834,10.777l5.666,-17.438l-14.833,-10.777l18.335,0l5.666,-17.438c1.888,5.813 3.777,11.625 5.666,17.438Zm-8.407,4.026l-8.869,0l7.175,5.213l-2.74,8.435l7.175,-5.213l7.175,5.213l-2.741,-8.435l7.175,-5.213l-8.869,0l-2.74,-8.434c-0.914,2.811 -1.827,5.623 -2.741,8.434Z" fill-rule="nonzero"/></svg>)svg"},
        {"star-svgrepo-com.svg", R"svg(<svg viewBox="0 0 32 32" fill="currentColor"><path d="M16 4.588l2.833 8.719H28l-7.416 5.387 2.832 8.719L16 22.023l-7.417 5.389 2.833-8.719L4 13.307h9.167L16 4.588z"/></svg>)svg"},
        {"stop_circle_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><circle cx="12" cy="12" r="10"/><rect x="9" y="9" width="6" height="6" rx="1" fill="white"/></svg>)svg"},
        {"sun_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><circle cx="12" cy="12" r="5"/><line x1="12" y1="1" x2="12" y2="3" stroke="currentColor" stroke-width="2" stroke-linecap="round"/><line x1="12" y1="21" x2="12" y2="23" stroke="currentColor" stroke-width="2" stroke-linecap="round"/><line x1="4.22" y1="4.22" x2="5.64" y2="5.64" stroke="currentColor" stroke-width="2" stroke-linecap="round"/><line x1="18.36" y1="18.36" x2="19.78" y2="19.78" stroke="currentColor" stroke-width="2" stroke-linecap="round"/><line x1="1" y1="12" x2="3" y2="12" stroke="currentColor" stroke-width="2" stroke-linecap="round"/><line x1="21" y1="12" x2="23" y2="12" stroke="currentColor" stroke-width="2" stroke-linecap="round"/><line x1="4.22" y1="19.78" x2="5.64" y2="18.36" stroke="currentColor" stroke-width="2" stroke-linecap="round"/><line x1="18.36" y1="5.64" x2="19.78" y2="4.22" stroke="currentColor" stroke-width="2" stroke-linecap="round"/></svg>)svg"},
        {"table_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><rect x="3" y="3" width="18" height="18" rx="2"/><path d="M3 9h18M9 9v12" fill="none" stroke="white" stroke-width="1.5"/></svg>)svg"},
        {"tag_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><path d="M21.41 11.58l-9-9A2 2 0 0 0 11 2H4a2 2 0 0 0-2 2v7a2 2 0 0 0 .59 1.42l9 9a2 2 0 0 0 2.82 0l7-7a2 2 0 0 0 0-2.84z"/><circle cx="6.5" cy="6.5" r="1.5" fill="#fff"/></svg>)svg"},
        {"today_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><rect x="3" y="4" width="18" height="18" rx="2"/><path d="M3 10h18" fill="none" stroke="white" stroke-width="1.5"/><path d="M8 2v4M16 2v4" fill="none" stroke="white" stroke-width="1.5"/><rect x="9" y="14" width="6" height="5" rx="1" fill="white"/></svg>)svg"},
        {"trash_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><polyline points="3 6 5 6 21 6" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round"/><path d="M19 6l-1 14a2 2 0 0 1-2 2H8a2 2 0 0 1-2-2L5 6"/><path d="M10 11v6M14 11v6" fill="none" stroke="white" stroke-width="1.5" stroke-linecap="round"/><path d="M9 6V4a1 1 0 0 1 1-1h4a1 1 0 0 1 1 1v2" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round"/></svg>)svg"},
        {"user_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><circle cx="12" cy="8" r="4"/><path d="M4 20c0-4 3.58-7 8-7s8 3 8 7"/></svg>)svg"},
        {"users_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><circle cx="9" cy="7" r="4"/><path d="M1 20c0-3.31 3.13-6 7-6s7 2.69 7 6"/><circle cx="17" cy="8" r="3"/><path d="M23 20c0-2.76-2.24-5-5-5"/></svg>)svg"},
        {"video_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><polygon points="23 7 16 12 23 17"/><rect x="1" y="5" width="15" height="14" rx="2"/></svg>)svg"},
        {"volume_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><polygon points="11 5 6 9 2 9 2 15 6 15 11 19"/><path d="M15.54 8.46a5 5 0 0 1 0 7.07M19.07 4.93a10 10 0 0 1 0 14.14" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round"/></svg>)svg"},
        {"wand_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><path d="M15 4l5 5L7 21l-5-5z"/><circle cx="4" cy="4" r="1.2"/><circle cx="20" cy="16" r="1.2"/><circle cx="16" cy="2" r="1"/><circle cx="22" cy="10" r="1"/><circle cx="10" cy="2" r="0.8"/></svg>)svg"},
        {"x_circle_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><circle cx="12" cy="12" r="10"/><line x1="15" y1="9" x2="9" y2="15" stroke="white" stroke-width="2" stroke-linecap="round"/><line x1="9" y1="9" x2="15" y2="15" stroke="white" stroke-width="2" stroke-linecap="round"/></svg>)svg"},
        {"zap_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><polygon points="13 2 3 14 12 14 11 22 21 10 12 10 13 2"/></svg>)svg"},
        {"ferrex", R"svg(<svg version="1.1" xmlns="http://www.w3.org/2000/svg" style="display: block;" viewBox="0 0 2048 2048" preserveAspectRatio="none">
<path transform="translate(0,0)" fill="rgb(254,139,2)" d="M 1018.55 41.3707 C 1030.64 41.0001 1041.04 40.6665 1052.32 46.1858 C 1080.7 60.0737 1108.29 76.8037 1135.69 92.3246 L 1307.97 191.34 L 1482.18 291.092 C 1596.32 358.159 1711.07 424.184 1826.41 489.16 C 1888.65 523.756 1888.34 535.762 1887.05 604.498 C 1886.86 618.766 1886.79 633.036 1886.83 647.305 L 1885.93 877.427 L 1885.37 1273.38 L 1885.67 1409.39 C 1885.76 1434.47 1886.15 1460.24 1885.58 1484.91 C 1885.09 1506.3 1869.53 1527.55 1851.77 1538.23 C 1824.31 1554.73 1796 1571.19 1768.12 1586.91 L 1668.65 1643.66 L 1390.77 1804.2 L 1153.93 1941.33 L 1098.44 1973.45 C 1077.34 1985.82 1057.9 1999.63 1034.13 2006 C 1022.32 2006.51 1012.95 2006.98 1001.58 2002.96 C 988.028 1998.17 975.821 1991.03 964.017 1982.88 C 958.492 1979.06 952.851 1974.14 947.039 1970.88 C 911.647 1951.02 876.373 1930.61 841.194 1910.39 L 484.592 1706.45 L 269.365 1584.39 L 217.338 1554.3 C 159.544 1520.89 161.059 1514.38 161.075 1448.75 L 161.137 1366.93 L 161.655 1094.5 L 161.815 735.737 L 161.878 621.812 C 161.892 601.398 161.349 579.776 162.37 559.52 C 163.326 540.552 180.647 517.613 196.656 508.311 C 227.386 490.455 258.738 473.729 289.346 455.729 L 608.947 270.751 C 738.455 196.445 866.757 119.783 997.143 46.9686 C 1004.27 42.9896 1010.69 42.2408 1018.55 41.3707 z M 1043.52 1310.27 C 1201.76 1299.65 1321.45 1162.79 1310.91 1004.55 C 1300.37 846.297 1163.58 726.532 1005.32 736.993 C 846.951 747.461 727.078 884.374 737.629 1042.74 C 748.18 1201.11 885.156 1320.91 1043.52 1310.27 z M 510.92 1237.11 C 531.131 1223.77 556.427 1212.19 576.233 1198.92 C 568.779 1183.17 566.923 1173.77 562.256 1157.2 C 535.967 1063.85 537.946 957.019 569.392 865.069 C 571.352 859.337 574.087 853.415 576.689 847.981 C 540.62 826.461 502.151 806.241 465.97 784.491 C 446.673 772.89 419.414 753.14 396.713 754.484 C 381.031 756.245 372.591 762.284 361.931 773.982 C 358.515 777.731 354.277 785.854 354.039 790.995 C 352.775 818.348 353.109 846.828 353.071 874.224 L 353.018 1036.61 L 353.063 1181.74 C 353.08 1206.36 352.586 1231.18 353.889 1255.81 C 354.426 1265.95 363.933 1276.09 371.39 1282.5 C 382.386 1291.95 407.904 1295.94 421.165 1288.95 C 440.009 1279.03 459.381 1265.71 478.081 1255.19 C 485.629 1250.62 505.412 1241 510.92 1237.11 z M 1095.82 1499.6 L 1095.73 1625.78 C 1095.72 1636.83 1095.6 1648.04 1095.74 1659.02 C 1095.99 1678.86 1093.48 1691.02 1108.9 1706.49 C 1122.59 1720.22 1141.76 1725.05 1160.41 1718.85 C 1170.04 1715.31 1179.83 1707.42 1188.96 1702.59 C 1202.47 1694.32 1215.79 1686.84 1229.43 1678.95 C 1260 1662.85 1295.18 1641.42 1325.32 1623.92 L 1455.56 1548.47 C 1465.73 1542.49 1477.95 1536.04 1488.11 1529.75 C 1509.28 1516.65 1565.56 1493.17 1572.94 1468.98 C 1586.21 1425.52 1551.36 1411.4 1520.88 1393.68 L 1466.05 1361.88 C 1462.5 1359.86 1398.42 1322.34 1402.26 1323.07 C 1395.77 1325.75 1390.53 1335.11 1385.51 1340.51 C 1374.91 1351.91 1364.08 1363.12 1353.63 1374.67 C 1306.59 1420.02 1242.6 1457.24 1181.21 1479.13 C 1157.29 1487.67 1119.82 1494.3 1095.82 1499.6 z M 1471.7 849.077 C 1513.28 934.051 1514.1 1079.03 1483.79 1168.27 C 1480.24 1178.47 1476.4 1188.56 1472.28 1198.54 C 1488.92 1210.93 1528.46 1230.53 1548.38 1242.39 C 1573.28 1257.02 1598.92 1270.38 1623.48 1285.78 C 1641.26 1296.93 1661.16 1294.1 1677.31 1281.27 C 1687.58 1273.11 1694.09 1262.21 1694.23 1249.03 C 1694.86 1226.88 1694.67 1204.71 1694.65 1182.53 L 1694.65 1069.87 L 1694.79 884.673 C 1694.78 865.512 1694.83 846.424 1694.68 827.15 C 1694.48 799.565 1698.18 772.863 1668.1 759.118 C 1659.19 755.044 1647.66 754.056 1637.99 755.299 C 1623.67 759.776 1608.59 769.42 1595.37 777.061 L 1547.56 804.761 C 1523.74 818.522 1493.99 833.826 1471.7 849.077 z M 1140.94 326.474 C 1089.06 334.998 1096.73 367.689 1096.33 408.347 C 1096.27 414.839 1096.41 421.446 1096.46 427.925 L 1097.03 547.749 C 1190.74 561.155 1278.22 602.552 1348 666.517 C 1358.83 676.4 1369.58 686.772 1379.65 697.433 C 1387.39 705.617 1394.63 716.138 1403.28 723.139 C 1417.99 711.107 1463.83 686.551 1483.1 675.494 C 1508.39 661.888 1536.54 644.836 1560.43 628.522 C 1576.92 617.256 1578.58 586.469 1568.92 569.919 C 1558.31 553.737 1522.22 537.596 1504.41 526.98 C 1436.04 486.492 1367.18 446.85 1297.83 408.065 C 1274.78 394.878 1171.65 331.424 1154.71 327.552 C 1150.18 326.517 1145.56 326.261 1140.94 326.474 z M 821.524 1680.36 L 822.691 1680.93 C 846.176 1692.55 869.08 1710.19 892.898 1719.5 C 911.699 1726.85 940.172 1713.82 947.651 1695.46 C 949.898 1689.94 951.348 1684.27 951.63 1678.23 C 952.151 1652.95 952.365 1626.14 951.892 1600.89 C 951.262 1567.23 952.607 1531.9 951.593 1498.5 C 933.804 1495.28 916.835 1493 899.209 1488.38 C 810.523 1465.15 729.749 1415.13 668.24 1347.26 C 663.522 1342.05 652.13 1327.98 646.991 1323.97 C 633.729 1333.92 614.187 1344.04 599.2 1352.45 C 573.299 1366.85 547.54 1381.51 521.929 1396.42 C 510.328 1403.29 497.188 1410.67 487.212 1418.73 C 471.969 1431.05 467.431 1463.97 480.812 1478.52 C 497.448 1496.62 528.687 1511.73 550.044 1524.11 L 668.398 1591.56 C 699.384 1609.27 731.365 1628.2 762.167 1646.24 C 775.065 1653.79 811.365 1672.68 821.524 1680.36 z M 573.2 679.578 C 586.68 686.68 598.636 694.927 611.6 702.755 C 623.256 709.793 635.717 716.476 647.702 722.978 C 655.447 715.553 660.983 707.483 667.614 701.002 C 681.56 687.372 698.212 669.05 712.765 656.522 C 768.844 608.243 839.27 572.16 911.095 554.442 C 921.958 551.762 940.697 549.534 951.928 547.992 C 951.642 507.416 951.634 466.839 951.905 426.263 C 951.966 409.445 953.528 376.465 950.362 360.596 C 945.431 335.88 920.87 323.542 895.832 327.342 C 892.106 328.089 886.144 329.264 883.014 331.139 C 859.685 345.114 835.963 358.964 812.416 372.545 L 654.007 463.802 L 533.421 533.125 C 517.401 542.539 489.499 555.37 479.431 570.355 C 464.335 592.824 475.664 624.393 498.583 636.68 C 523.469 652.235 549.461 664.469 573.2 679.578 z"/>
<path transform="translate(0,0)" fill="rgb(10,7,13)" fill-opacity="0.0117647" d="M 1489.8 1492.45 L 1491.03 1493.33 C 1492.45 1495.89 1492.12 1500.5 1492.17 1503.68 C 1489.15 1501.35 1489.9 1496.22 1489.8 1492.45 z"/>
</svg>)svg"},
        {"panel_sidebar_filled", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.8" stroke-linecap="round"><rect x="2" y="2" width="20" height="20" rx="4"/><clipPath id="c"><rect x="2" y="2" width="20" height="20" rx="4"/></clipPath><rect x="2" y="2" width="10" height="20" fill="currentColor" stroke="none" clip-path="url(#c)"/><line x1="12" y1="2" x2="12" y2="22"/></svg>)svg"},
        {"panel_split", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><rect x="2" y="2" width="9.5" height="20" rx="3"/><rect x="12.5" y="2" width="9.5" height="20" rx="3"/></svg>)svg"},
        {"bell", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M18 8A6 6 0 0 0 6 8c0 7-3 9-3 9h18s-3-2-3-9"></path><path d="M13.73 21a2 2 0 0 1-3.46 0"></path></svg>)svg"},
        {"switch", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M12 2v20M2 12h20"/></svg>)svg"},
        {"panel_sidebar", R"svg(<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.8" stroke-linecap="round"><rect x="2" y="2" width="20" height="20" rx="4"/><line x1="12" y1="2" x2="12" y2="22"/></svg>)svg"},
        {"reset-svgrepo-com", R"svg(<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg width="800px" height="800px" viewBox="0 0 512 512" version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
    <title>reset</title>
    <g id="Page-1" stroke="none" stroke-width="1" fill="none" fill-rule="evenodd">
        <g id="Combined-Shape" fill="currentColor" transform="translate(74.806872, 64.000000)">
            <path d="M351.859794,42.6666667 L351.859794,85.3333333 L283.193855,85.3303853 C319.271288,116.988529 341.381875,163.321355 341.339886,213.803851 C341.27474,291.98295 288.098183,360.121539 212.277591,379.179704 C136.456999,398.237869 57.3818117,363.341907 20.3580507,294.485411 C-16.6657103,225.628916 -2.17003698,140.420413 55.5397943,87.68 C63.6931909,100.652227 75.1888658,111.189929 88.8197943,118.186667 C59.4998648,141.873553 42.4797783,177.560832 42.5264609,215.253333 C43.5757012,285.194843 100.577082,341.341203 170.526461,341.333333 C234.598174,342.388718 289.235113,295.138227 297.4321,231.584253 C303.556287,184.101393 282.297007,138.84385 245.195596,112.637083 L245.193128,192 L202.526461,192 L202.526461,42.6666667 L351.859794,42.6666667 Z M127.859794,-1.42108547e-14 C151.423944,-1.42108547e-14 170.526461,19.1025173 170.526461,42.6666667 C170.526461,66.230816 151.423944,85.3333333 127.859794,85.3333333 C104.295645,85.3333333 85.1931276,66.230816 85.1931276,42.6666667 C85.1931276,19.1025173 104.295645,-1.42108547e-14 127.859794,-1.42108547e-14 Z">

</path>
        </g>
    </g>
</svg>)svg"},
        {"sparkles_filled", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" stroke="none"><path d="M12 2l2.4 7.4H22l-6.2 4.5 2.4 7.4L12 17l-6.2 4.3 2.4-7.4L2 9.4h7.6z"/></svg>)svg"},
        {"menu_triangle", R"svg(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24"><path fill="currentColor" d="M10 17l5-5-5-5v10z"/></svg>)svg"},
        {"dropdown_triangle", R"svg(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 12 12"><path fill="currentColor" d="M2 4.5l4 4l4-4z"/></svg>)svg"},
        {"invalid_data", R"svg(<svg viewBox="0 0 48 48" version="1" xmlns="http://www.w3.org/2000/svg"><circle fill="currentColor" cx="17" cy="17" r="14"/><circle fill="currentColor" cx="17" cy="17" r="11"/><rect x="16" y="8" width="2" height="9"/><rect x="18.2" y="16" transform="matrix(-.707 .707 -.707 -.707 46.834 19.399)" width="2.4" height="6.8"/><circle cx="17" cy="17" r="2"/><circle fill="currentColor" cx="17" cy="17" r="1"/><path fill="currentColor" d="M11.9,42l14.4-24.1c0.8-1.3,2.7-1.3,3.4,0L44.1,42c0.8,1.3-0.2,3-1.7,3H13.6C12.1,45,11.1,43.3,11.9,42z"/><path fill="currentColor" d="M26.4,39.9c0-0.2,0-0.4,0.1-0.6s0.2-0.3,0.3-0.5s0.3-0.2,0.5-0.3s0.4-0.1,0.6-0.1s0.5,0,0.7,0.1 s0.4,0.2,0.5,0.3s0.2,0.3,0.3,0.5s0.1,0.4,0.1,0.6s0,0.4-0.1,0.6s-0.2,0.3-0.3,0.5s-0.3,0.2-0.5,0.3s-0.4,0.1-0.7,0.1 s-0.5,0-0.6-0.1s-0.4-0.2-0.5-0.3s-0.2-0.3-0.3-0.5S26.4,40.1,26.4,39.9z M29.2,36.8h-2.3L26.5,27h3L29.2,36.8z"/></svg>)svg"},
        {"reset_filter", R"svg(<svg viewBox="0 0 5.82 5.82" fill="currentColor"><g transform="translate(0 -291.18)"><path d="M2.646 293.56v.53l1.058 1.058v1.323l1.058.529v-1.852l1.059-1.058v-.53zm.529.53h2.117l-.794.793v1.588l-.53-.265v-1.323z" fill-rule="evenodd"/><g transform="matrix(.26458 0 0 .26458 -1.058 18.033)"><path d="M15 1035.362v2h-4a5 5 0 0 0 0 10h2v2h-2a7 7 0 0 1-7-7c0-3.866 3.134-7.06 7-7z"/><path d="m17 1036.362-4 4v-8z"/></g></g></svg>)svg"},
        {"filter_funnel_outline", R"svg(<svg viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg"><path d="M3 4.6C3 4.03995 3 3.75992 3.10899 3.54601C3.20487 3.35785 3.35785 3.20487 3.54601 3.10899C3.75992 3 4.03995 3 4.6 3H19.4C19.9601 3 20.2401 3 20.454 3.10899C20.6422 3.20487 20.7951 3.35785 20.891 3.54601C21 3.75992 21 4.03995 21 4.6V6.33726C21 6.58185 21 6.70414 20.9724 6.81923C20.9479 6.92127 20.9075 7.01881 20.8526 7.10828C20.7908 7.2092 20.7043 7.29568 20.5314 7.46863L14.4686 13.5314C14.2957 13.7043 14.2092 13.7908 14.1474 13.8917C14.0925 13.9812 14.0521 14.0787 14.0276 14.1808C14 14.2959 14 14.4182 14 14.6627V17L10 21V14.6627C10 14.4182 10 14.2959 9.97237 14.1808C9.94787 14.0787 9.90747 13.9812 9.85264 13.8917C9.7908 13.7908 9.70432 13.7043 9.53137 13.5314L3.46863 7.46863C3.29568 7.29568 3.2092 7.2092 3.14736 7.10828C3.09253 7.01881 3.05213 6.92127 3.02763 6.81923C3 6.70414 3 6.58185 3 6.33726V4.6Z" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"/></svg>)svg"},
        {"prohibit", R"svg(<svg viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg" fill="none"><path stroke="currentColor" stroke-width="2" d="M5.5 5.5L18.5 18.5M21 12C21 16.9706 16.9706 21 12 21C7.02944 21 3 16.9706 3 12C3 7.02944 7.02944 3 12 3C16.9706 3 21 7.02944 21 12Z"/></svg>)svg"},
        {"restore_line", R"svg(<svg viewBox="0 0 24 24" fill="currentColor" fill-rule="evenodd"><path d="M19,3 C20.0543909,3 20.9181678,3.81587733 20.9945144,4.85073759 L21,5 L21,15 C21,16.0543909 20.18415,16.9181678 19.1492661,16.9945144 L19,17 L17,17 L17,19 C17,20.0543909 16.18415,20.9181678 15.1492661,20.9945144 L15,21 L5,21 C3.94563773,21 3.08183483,20.18415 3.00548573,19.1492661 L3,19 L3,9 C3,7.94563773 3.81587733,7.08183483 4.85073759,7.00548573 L5,7 L7,7 L7,5 C7,3.94563773 7.81587733,3.08183483 8.85073759,3.00548573 L9,3 L19,3 Z M15,9 L5,9 L5,19 L15,19 L15,9 Z M19,5 L9,5 L9,7 L15,7 L15.1492661,7.00548573 C16.1324058,7.07801738 16.9178674,7.86122607 16.9939557,8.84334947 L17,9 L17,15 L19,15 L19,5 Z"/></svg>)svg"},

        {"archive-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools --> <svg fill="currentColor" width="800px" height="800px" viewBox="0 0 1920 1920" xmlns="http://www.w3.org/2000/svg"> <path d="M533.333 560v160H240l.008 453.33 209.066 213.34H1470.93l209.06-213.34L1680 720h-293.33V560h352c55.96 0 101.33 45.368 101.33 101.333v511.997h16c35.35 0 64 28.66 64 64V1856c0 35.35-28.65 64-64 64H64c-35.346 0-64-28.65-64-64v-618.67c0-35.34 28.654-64 64-64h16V661.333C80 605.368 125.369 560 181.333 560h352ZM1040 0v958.86l183.43-183.429 113.14 113.138L960 1265.14 583.431 888.569l113.138-113.138L880 958.86V0h160Z"/> </svg>)svg"},
        {"archive-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools --> <svg fill="currentColor" width="800px" height="800px" viewBox="0 0 1920 1920" xmlns="http://www.w3.org/2000/svg"> <path d="M533.333 560v160H240l.008 453.33 209.066 213.34H1470.93l209.06-213.34L1680 720h-293.33V560h352c55.96 0 101.33 45.368 101.33 101.333v511.997h16c35.35 0 64 28.66 64 64V1856c0 35.35-28.65 64-64 64H64c-35.346 0-64-28.65-64-64v-618.67c0-35.34 28.654-64 64-64h16V661.333C80 605.368 125.369 560 181.333 560h352ZM1040 0v958.86l183.43-183.429 113.14 113.138L960 1265.14 583.431 888.569l113.138-113.138L880 958.86V0h160Z"/> </svg>)svg"},
        {"arrow_down.svg", R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="12" height="12" viewBox="0 0 12 12"> <path d="M3 4l3 4 3-4" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"/> </svg>)svg"},
        {"arrow_right.svg", R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="12" height="12" viewBox="0 0 12 12"> <path d="M4 3l4 3-4 3" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"/> </svg>)svg"},
        {"arrow_right", R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="12" height="12" viewBox="0 0 12 12"> <path d="M4 3l4 3-4 3" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"/> </svg>)svg"},
        {"document-attach-outline.svg", R"svg(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 512 512" class="ionicon"><path d="M208 64h66.75a32 32 0 0 1 22.62 9.37l141.26 141.26a32 32 0 0 1 9.37 22.62V432a48 48 0 0 1-48 48H192a48 48 0 0 1-48-48V304" fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="32px"/><path d="M288 72v120a32 32 0 0 0 32 32h120" fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="32px"/><path d="M160 80v152a23.69 23.69 0 0 1-24 24c-12 0-24-9.1-24-24V88c0-30.59 16.57-56 48-56s48 24.8 48 55.38v138.75c0 43-27.82 77.87-72 77.87s-72-34.86-72-77.87V144" fill="none" stroke="currentColor" stroke-linecap="round" stroke-miterlimit="10" stroke-width="32px"/></svg>)svg"},
        {"document-attach-outline", R"svg(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 512 512" class="ionicon"><path d="M208 64h66.75a32 32 0 0 1 22.62 9.37l141.26 141.26a32 32 0 0 1 9.37 22.62V432a48 48 0 0 1-48 48H192a48 48 0 0 1-48-48V304" fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="32px"/><path d="M288 72v120a32 32 0 0 0 32 32h120" fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="32px"/><path d="M160 80v152a23.69 23.69 0 0 1-24 24c-12 0-24-9.1-24-24V88c0-30.59 16.57-56 48-56s48 24.8 48 55.38v138.75c0 43-27.82 77.87-72 77.87s-72-34.86-72-77.87V144" fill="none" stroke="currentColor" stroke-linecap="round" stroke-miterlimit="10" stroke-width="32px"/></svg>)svg"},
        {"download-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools --> <svg fill="currentColor" width="800px" height="800px" viewBox="0 0 1920 1920" xmlns="http://www.w3.org/2000/svg"> <path d="M1764.098 1355.412 1920 1511.314l-363.073 363.073H363.073L0 1511.314l155.902-155.902 298.463 298.463h1011.27l298.463-298.463ZM1070.333 0v949.967l250.502-250.612 155.902 155.902-518.975 518.975-518.976-518.975 155.902-155.902 255.023 255.022V0h220.622Z" fill-rule="evenodd"/> </svg>)svg"},
        {"download-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools --> <svg fill="currentColor" width="800px" height="800px" viewBox="0 0 1920 1920" xmlns="http://www.w3.org/2000/svg"> <path d="M1764.098 1355.412 1920 1511.314l-363.073 363.073H363.073L0 1511.314l155.902-155.902 298.463 298.463h1011.27l298.463-298.463ZM1070.333 0v949.967l250.502-250.612 155.902 155.902-518.975 518.975-518.976-518.975 155.902-155.902 255.023 255.022V0h220.622Z" fill-rule="evenodd"/> </svg>)svg"},
        {"flag-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools --> <svg fill="currentColor" width="800px" height="800px" viewBox="0 0 1920 1920" xmlns="http://www.w3.org/2000/svg"> <path d="M168.941-.011v1920H56v-1920h112.941Zm112.941 68.453c308.669-81.656 496.15 26.429 677.196 133.045 203.407 119.944 413.59 244.066 833.844 139.03 20.217-4.969 41.676 1.469 55.793 17.168 13.892 15.699 18.07 37.835 10.843 57.487-203.407 542.343-504.17 552.734-794.993 562.786-223.285 7.906-454.25 15.811-686.344 247.906l-96.339 96.338Z" fill-rule="evenodd"/> </svg>)svg"},
        {"flag-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools --> <svg fill="currentColor" width="800px" height="800px" viewBox="0 0 1920 1920" xmlns="http://www.w3.org/2000/svg"> <path d="M168.941-.011v1920H56v-1920h112.941Zm112.941 68.453c308.669-81.656 496.15 26.429 677.196 133.045 203.407 119.944 413.59 244.066 833.844 139.03 20.217-4.969 41.676 1.469 55.793 17.168 13.892 15.699 18.07 37.835 10.843 57.487-203.407 542.343-504.17 552.734-794.993 562.786-223.285 7.906-454.25 15.811-686.344 247.906l-96.339 96.338Z" fill-rule="evenodd"/> </svg>)svg"},
        {"image-picture-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="UTF-8" standalone="no"?> <!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools --> <svg width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" xmlns:sketch="http://www.bohemiancoding.com/sketch/ns"> <title>image-picture</title> <desc>Created with Sketch Beta.</desc> <defs> </defs> <g id="Page-1" stroke="none" stroke-width="1" fill="none" fill-rule="evenodd" sketch:type="MSPage"> <g id="Icon-Set-Filled" sketch:type="MSLayerGroup" transform="translate(-362.000000, -101.000000)" fill="currentColor"> <path d="M392,129 C392,130.104 391.104,131 390,131 L384.832,131 L377.464,123.535 L386,114.999 L392,120.999 L392,129 L392,129 Z M366,131 C364.896,131 364,130.104 364,129 L364,128.061 L371.945,120.945 L382.001,131 L366,131 L366,131 Z M370,105 C372.209,105 374,106.791 374,109 C374,111.209 372.209,113 370,113 C367.791,113 366,111.209 366,109 C366,106.791 367.791,105 370,105 L370,105 Z M390,101 L366,101 C363.791,101 362,102.791 362,105 L362,129 C362,131.209 363.791,133 366,133 L390,133 C392.209,133 394,131.209 394,129 L394,105 C394,102.791 392.209,101 390,101 L390,101 Z M370,111 C371.104,111 372,110.104 372,109 C372,107.896 371.104,107 370,107 C368.896,107 368,107.896 368,109 C368,110.104 368.896,111 370,111 L370,111 Z" id="image-picture" sketch:type="MSShapeGroup"> </path> </g> </g> </svg>)svg"},
        {"image-picture-svgrepo-com", R"svg(<?xml version="1.0" encoding="UTF-8" standalone="no"?> <!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools --> <svg width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" xmlns:sketch="http://www.bohemiancoding.com/sketch/ns"> <title>image-picture</title> <desc>Created with Sketch Beta.</desc> <defs> </defs> <g id="Page-1" stroke="none" stroke-width="1" fill="none" fill-rule="evenodd" sketch:type="MSPage"> <g id="Icon-Set-Filled" sketch:type="MSLayerGroup" transform="translate(-362.000000, -101.000000)" fill="currentColor"> <path d="M392,129 C392,130.104 391.104,131 390,131 L384.832,131 L377.464,123.535 L386,114.999 L392,120.999 L392,129 L392,129 Z M366,131 C364.896,131 364,130.104 364,129 L364,128.061 L371.945,120.945 L382.001,131 L366,131 L366,131 Z M370,105 C372.209,105 374,106.791 374,109 C374,111.209 372.209,113 370,113 C367.791,113 366,111.209 366,109 C366,106.791 367.791,105 370,105 L370,105 Z M390,101 L366,101 C363.791,101 362,102.791 362,105 L362,129 C362,131.209 363.791,133 366,133 L390,133 C392.209,133 394,131.209 394,129 L394,105 C394,102.791 392.209,101 390,101 L390,101 Z M370,111 C371.104,111 372,110.104 372,109 C372,107.896 371.104,107 370,107 C368.896,107 368,107.896 368,109 C368,110.104 368.896,111 370,111 L370,111 Z" id="image-picture" sketch:type="MSShapeGroup"> </path> </g> </g> </svg>)svg"},
        {"microsoftonenote-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?> <!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools --> <svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg"> <title>microsoftonenote</title> <path d="M29.125 10.375v-5.625h-18.75v3.75h4.375c0.683 0.017 1.233 0.567 1.25 1.248l0 0.002v12.5c-0.017 0.683-0.567 1.233-1.248 1.25l-0.002 0h-4.375v3.75h13.125v-16.875zM29.125 16v-3.75h-3.75v3.75zM29.125 21.625v-3.75h-3.75v3.75zM29.125 27.25v-3.75h-3.75v3.75zM6.7 14.75l3.538 6.163h2.238v-9.825h-2.175v6.287l-3.412-6.287h-2.362v9.825h2.175zM29.75 2.875c0.005-0 0.010-0 0.015-0 0.339 0 0.645 0.144 0.859 0.374l0.001 0.001c0.231 0.215 0.375 0.52 0.375 0.86 0 0.005-0 0.011-0 0.016v-0.001 23.75c-0.017 0.683-0.567 1.233-1.248 1.25l-0.002 0h-20c-0.683-0.017-1.233-0.567-1.25-1.248l-0-0.002v-4.375h-6.25c-0.005 0-0.010 0-0.015 0-0.339 0-0.645-0.144-0.859-0.374l-0.001-0.001c-0.231-0.215-0.375-0.52-0.375-0.86 0-0.005 0-0.011 0-0.016v0.001-12.5c-0-0.005-0-0.010-0-0.015 0-0.339 0.144-0.645 0.374-0.859l0.001-0.001c0.211-0.231 0.513-0.375 0.849-0.375 0.009 0 0.018 0 0.028 0l-0.001-0h6.25v-4.375c-0-0.005-0-0.010-0-0.015 0-0.339 0.144-0.645 0.374-0.859l0.001-0.001c0.215-0.231 0.52-0.375 0.86-0.375 0.005 0 0.011 0 0.016 0h-0.001z"></path> </svg>)svg"},
        {"microsoftonenote-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?> <!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools --> <svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg"> <title>microsoftonenote</title> <path d="M29.125 10.375v-5.625h-18.75v3.75h4.375c0.683 0.017 1.233 0.567 1.25 1.248l0 0.002v12.5c-0.017 0.683-0.567 1.233-1.248 1.25l-0.002 0h-4.375v3.75h13.125v-16.875zM29.125 16v-3.75h-3.75v3.75zM29.125 21.625v-3.75h-3.75v3.75zM29.125 27.25v-3.75h-3.75v3.75zM6.7 14.75l3.538 6.163h2.238v-9.825h-2.175v6.287l-3.412-6.287h-2.362v9.825h2.175zM29.75 2.875c0.005-0 0.010-0 0.015-0 0.339 0 0.645 0.144 0.859 0.374l0.001 0.001c0.231 0.215 0.375 0.52 0.375 0.86 0 0.005-0 0.011-0 0.016v-0.001 23.75c-0.017 0.683-0.567 1.233-1.248 1.25l-0.002 0h-20c-0.683-0.017-1.233-0.567-1.25-1.248l-0-0.002v-4.375h-6.25c-0.005 0-0.010 0-0.015 0-0.339 0-0.645-0.144-0.859-0.374l-0.001-0.001c-0.231-0.215-0.375-0.52-0.375-0.86 0-0.005 0-0.011 0-0.016v0.001-12.5c-0-0.005-0-0.010-0-0.015 0-0.339 0.144-0.645 0.374-0.859l0.001-0.001c0.211-0.231 0.513-0.375 0.849-0.375 0.009 0 0.018 0 0.028 0l-0.001-0h6.25v-4.375c-0-0.005-0-0.010-0-0.015 0-0.339 0.144-0.645 0.374-0.859l0.001-0.001c0.215-0.231 0.52-0.375 0.86-0.375 0.005 0 0.011 0 0.016 0h-0.001z"></path> </svg>)svg"},
        {"note-1.svg", R"svg(<svg width="20" height="20" viewBox="0 0 20 20" fill="none" xmlns="http://www.w3.org/2000/svg"> <path d="M5.5 12.5H11.5V14H5.5V12.5Z" fill="currentColor"/> <path d="M5.5 9.25H14.5V10.75H5.5V9.25Z" fill="currentColor"/> <path d="M5.5 6H14.5V7.5H5.5V6Z" fill="currentColor"/> <path fill-rule="evenodd" clip-rule="evenodd" d="M14 3.5H6C4.61929 3.5 3.5 4.61929 3.5 6V14C3.5 15.3807 4.61929 16.5 6 16.5H14C15.3807 16.5 16.5 15.3807 16.5 14V6C16.5 4.61929 15.3807 3.5 14 3.5ZM6 2C3.79086 2 2 3.79086 2 6V14C2 16.2091 3.79086 18 6 18H14C16.2091 18 18 16.2091 18 14V6C18 3.79086 16.2091 2 14 2H6Z" fill="currentColor"/> </svg>)svg"},
        {"note-1", R"svg(<svg width="20" height="20" viewBox="0 0 20 20" fill="none" xmlns="http://www.w3.org/2000/svg"> <path d="M5.5 12.5H11.5V14H5.5V12.5Z" fill="currentColor"/> <path d="M5.5 9.25H14.5V10.75H5.5V9.25Z" fill="currentColor"/> <path d="M5.5 6H14.5V7.5H5.5V6Z" fill="currentColor"/> <path fill-rule="evenodd" clip-rule="evenodd" d="M14 3.5H6C4.61929 3.5 3.5 4.61929 3.5 6V14C3.5 15.3807 4.61929 16.5 6 16.5H14C15.3807 16.5 16.5 15.3807 16.5 14V6C16.5 4.61929 15.3807 3.5 14 3.5ZM6 2C3.79086 2 2 3.79086 2 6V14C2 16.2091 3.79086 18 6 18H14C16.2091 18 18 16.2091 18 14V6C18 3.79086 16.2091 2 14 2H6Z" fill="currentColor"/> </svg>)svg"},
        {"note-2.svg", R"svg(<svg width="16" height="16" viewBox="0 0 16 16" xmlns="http://www.w3.org/2000/svg" fill="currentColor"><path d="M4.5 2C3.11929 2 2 3.11929 2 4.5V11.5C2 12.8807 3.11929 14 4.5 14H8.17157C8.83461 14 9.4705 13.7366 9.93934 13.2678L13.2678 9.93934C13.7366 9.4705 14 8.83461 14 8.17157V4.5C14 3.11929 12.8807 2 11.5 2H4.5ZM3 4.5C3 3.67157 3.67157 3 4.5 3H11.5C12.3284 3 13 3.67157 13 4.5V8H10.5C9.11929 8 8 9.11929 8 10.5V13H4.5C3.67157 13 3 12.3284 3 11.5V4.5ZM9 12.7505V10.5C9 9.67157 9.67157 9 10.5 9H12.7505C12.6955 9.08295 12.6321 9.16082 12.5607 9.23223L9.23223 12.5607C9.16082 12.6321 9.08295 12.6955 9 12.7505Z"/></svg>)svg"},
        {"note-2", R"svg(<svg width="16" height="16" viewBox="0 0 16 16" xmlns="http://www.w3.org/2000/svg" fill="currentColor"><path d="M4.5 2C3.11929 2 2 3.11929 2 4.5V11.5C2 12.8807 3.11929 14 4.5 14H8.17157C8.83461 14 9.4705 13.7366 9.93934 13.2678L13.2678 9.93934C13.7366 9.4705 14 8.83461 14 8.17157V4.5C14 3.11929 12.8807 2 11.5 2H4.5ZM3 4.5C3 3.67157 3.67157 3 4.5 3H11.5C12.3284 3 13 3.67157 13 4.5V8H10.5C9.11929 8 8 9.11929 8 10.5V13H4.5C3.67157 13 3 12.3284 3 11.5V4.5ZM9 12.7505V10.5C9 9.67157 9.67157 9 10.5 9H12.7505C12.6955 9.08295 12.6321 9.16082 12.5607 9.23223L9.23223 12.5607C9.16082 12.6321 9.08295 12.6955 9 12.7505Z"/></svg>)svg"},
        {"null.svg", R"svg(<svg fill="currentColor" id="Layer_1" data-name="Layer 1" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 16 16"><path d="M16,8A8,8,0,1,1,8,0,8,8,0,0,1,16,8ZM8,13.65a5.67,5.67,0,0,0,2.78-.74c.25-.14.23-.23,0-.41L3.49,5.15C3.31,5,3.23,5,3.11,5.2A5.64,5.64,0,0,0,8,13.65ZM13.65,8a7.37,7.37,0,0,0-.05-.82A5.67,5.67,0,0,0,5.18,3.11c-.18.11-.22.18,0,.35q3.72,3.7,7.42,7.42c.17.17.24.13.35-.06A5.62,5.62,0,0,0,13.65,8Z"/></svg>)svg"},
        {"null", R"svg(<svg fill="currentColor" id="Layer_1" data-name="Layer 1" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 16 16"><path d="M16,8A8,8,0,1,1,8,0,8,8,0,0,1,16,8ZM8,13.65a5.67,5.67,0,0,0,2.78-.74c.25-.14.23-.23,0-.41L3.49,5.15C3.31,5,3.23,5,3.11,5.2A5.64,5.64,0,0,0,8,13.65ZM13.65,8a7.37,7.37,0,0,0-.05-.82A5.67,5.67,0,0,0,5.18,3.11c-.18.11-.22.18,0,.35q3.72,3.7,7.42,7.42c.17.17.24.13.35-.06A5.62,5.62,0,0,0,13.65,8Z"/></svg>)svg"},
        {"paperclip-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools --> <svg fill="currentColor" width="800px" height="800px" viewBox="0 0 1920 1920" xmlns="http://www.w3.org/2000/svg"> <path d="M1748.318 252.722c-229.016-229.016-601.528-228.91-830.438 0l-792.748 792.749C44.522 1126.187 0 1233.38 0 1347.409c0 114.135 44.522 221.33 125.132 301.939 161.432 161.432 442.658 161.432 603.983 0l717.371-717.264c103.885-103.992 103.885-273.218-.213-377.53-104.099-103.992-273.646-103.885-377.424.107l-603.983 603.983 151.076 150.97 603.77-603.877c20.926-20.713 54.878-20.927 75.591-.214 20.82 20.927 20.82 54.879.107 75.698L578.146 1498.38c-80.716 80.716-221.329 80.716-302.045 0-40.358-40.358-62.566-93.956-62.566-150.97 0-57.013 22.208-110.61 62.566-150.969l792.748-792.749c145.631-145.417 382.655-145.63 528.5 0 145.63 145.631 145.63 382.869 0 528.5l-754.953 755.06 150.969 150.969 754.953-755.06c228.91-228.91 228.91-601.422 0-830.438" fill-rule="evenodd"/> </svg>)svg"},
        {"paperclip-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools --> <svg fill="currentColor" width="800px" height="800px" viewBox="0 0 1920 1920" xmlns="http://www.w3.org/2000/svg"> <path d="M1748.318 252.722c-229.016-229.016-601.528-228.91-830.438 0l-792.748 792.749C44.522 1126.187 0 1233.38 0 1347.409c0 114.135 44.522 221.33 125.132 301.939 161.432 161.432 442.658 161.432 603.983 0l717.371-717.264c103.885-103.992 103.885-273.218-.213-377.53-104.099-103.992-273.646-103.885-377.424.107l-603.983 603.983 151.076 150.97 603.77-603.877c20.926-20.713 54.878-20.927 75.591-.214 20.82 20.927 20.82 54.879.107 75.698L578.146 1498.38c-80.716 80.716-221.329 80.716-302.045 0-40.358-40.358-62.566-93.956-62.566-150.97 0-57.013 22.208-110.61 62.566-150.969l792.748-792.749c145.631-145.417 382.655-145.63 528.5 0 145.63 145.631 145.63 382.869 0 528.5l-754.953 755.06 150.969 150.969 754.953-755.06c228.91-228.91 228.91-601.422 0-830.438" fill-rule="evenodd"/> </svg>)svg"},
        {"trash-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="UTF-8" standalone="no"?> <!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools --> <svg width="800px" height="800px" viewBox="-3 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" xmlns:sketch="http://www.bohemiancoding.com/sketch/ns"> <title>trash</title> <desc>Created with Sketch Beta.</desc> <defs> </defs> <g id="Page-1" stroke="none" stroke-width="1" fill="none" fill-rule="evenodd" sketch:type="MSPage"> <g id="Icon-Set-Filled" sketch:type="MSLayerGroup" transform="translate(-261.000000, -205.000000)" fill="currentColor"> <path d="M268,220 C268,219.448 268.448,219 269,219 C269.552,219 270,219.448 270,220 L270,232 C270,232.553 269.552,233 269,233 C268.448,233 268,232.553 268,232 L268,220 L268,220 Z M273,220 C273,219.448 273.448,219 274,219 C274.552,219 275,219.448 275,220 L275,232 C275,232.553 274.552,233 274,233 C273.448,233 273,232.553 273,232 L273,220 L273,220 Z M278,220 C278,219.448 278.448,219 279,219 C279.552,219 280,219.448 280,220 L280,232 C280,232.553 279.552,233 279,233 C278.448,233 278,232.553 278,232 L278,220 L278,220 Z M263,233 C263,235.209 264.791,237 267,237 L281,237 C283.209,237 285,235.209 285,233 L285,217 L263,217 L263,233 L263,233 Z M277,209 L271,209 L271,208 C271,207.447 271.448,207 272,207 L276,207 C276.552,207 277,207.447 277,208 L277,209 L277,209 Z M285,209 L279,209 L279,207 C279,205.896 278.104,205 277,205 L271,205 C269.896,205 269,205.896 269,207 L269,209 L263,209 C261.896,209 261,209.896 261,211 L261,213 C261,214.104 261.895,214.999 262.999,215 L285.002,215 C286.105,214.999 287,214.104 287,213 L287,211 C287,209.896 286.104,209 285,209 L285,209 Z" id="trash" sketch:type="MSShapeGroup"> </path> </g> </g> </svg>)svg"},
        {"trash-svgrepo-com", R"svg(<?xml version="1.0" encoding="UTF-8" standalone="no"?> <!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools --> <svg width="800px" height="800px" viewBox="-3 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" xmlns:sketch="http://www.bohemiancoding.com/sketch/ns"> <title>trash</title> <desc>Created with Sketch Beta.</desc> <defs> </defs> <g id="Page-1" stroke="none" stroke-width="1" fill="none" fill-rule="evenodd" sketch:type="MSPage"> <g id="Icon-Set-Filled" sketch:type="MSLayerGroup" transform="translate(-261.000000, -205.000000)" fill="currentColor"> <path d="M268,220 C268,219.448 268.448,219 269,219 C269.552,219 270,219.448 270,220 L270,232 C270,232.553 269.552,233 269,233 C268.448,233 268,232.553 268,232 L268,220 L268,220 Z M273,220 C273,219.448 273.448,219 274,219 C274.552,219 275,219.448 275,220 L275,232 C275,232.553 274.552,233 274,233 C273.448,233 273,232.553 273,232 L273,220 L273,220 Z M278,220 C278,219.448 278.448,219 279,219 C279.552,219 280,219.448 280,220 L280,232 C280,232.553 279.552,233 279,233 C278.448,233 278,232.553 278,232 L278,220 L278,220 Z M263,233 C263,235.209 264.791,237 267,237 L281,237 C283.209,237 285,235.209 285,233 L285,217 L263,217 L263,233 L263,233 Z M277,209 L271,209 L271,208 C271,207.447 271.448,207 272,207 L276,207 C276.552,207 277,207.447 277,208 L277,209 L277,209 Z M285,209 L279,209 L279,207 C279,205.896 278.104,205 277,205 L271,205 C269.896,205 269,205.896 269,207 L269,209 L263,209 C261.896,209 261,209.896 261,211 L261,213 C261,214.104 261.895,214.999 262.999,215 L285.002,215 C286.105,214.999 287,214.104 287,213 L287,211 C287,209.896 286.104,209 285,209 L285,209 Z" id="trash" sketch:type="MSShapeGroup"> </path> </g> </g> </svg>)svg"},

        {"image_picture", R"svg(<?xml version="1.0" encoding="UTF-8" standalone="no"?> <!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools --> <svg width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" xmlns:sketch="http://www.bohemiancoding.com/sketch/ns"> <title>image-picture</title> <desc>Created with Sketch Beta.</desc> <defs> </defs> <g id="Page-1" stroke="none" stroke-width="1" fill="none" fill-rule="evenodd" sketch:type="MSPage"> <g id="Icon-Set-Filled" sketch:type="MSLayerGroup" transform="translate(-362.000000, -101.000000)" fill="currentColor"> <path d="M392,129 C392,130.104 391.104,131 390,131 L384.832,131 L377.464,123.535 L386,114.999 L392,120.999 L392,129 L392,129 Z M366,131 C364.896,131 364,130.104 364,129 L364,128.061 L371.945,120.945 L382.001,131 L366,131 L366,131 Z M370,105 C372.209,105 374,106.791 374,109 C374,111.209 372.209,113 370,113 C367.791,113 366,111.209 366,109 C366,106.791 367.791,105 370,105 L370,105 Z M390,101 L366,101 C363.791,101 362,102.791 362,105 L362,129 C362,131.209 363.791,133 366,133 L390,133 C392.209,133 394,131.209 394,129 L394,105 C394,102.791 392.209,101 390,101 L390,101 Z M370,111 C371.104,111 372,110.104 372,109 C372,107.896 371.104,107 370,107 C368.896,107 368,107.896 368,109 C368,110.104 368.896,111 370,111 L370,111 Z" id="image-picture" sketch:type="MSShapeGroup"> </path> </g> </g> </svg>)svg"},
        {"onenote", R"svg(<?xml version="1.0" encoding="utf-8"?> <!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools --> <svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg"> <title>microsoftonenote</title> <path d="M29.125 10.375v-5.625h-18.75v3.75h4.375c0.683 0.017 1.233 0.567 1.25 1.248l0 0.002v12.5c-0.017 0.683-0.567 1.233-1.248 1.25l-0.002 0h-4.375v3.75h13.125v-16.875zM29.125 16v-3.75h-3.75v3.75zM29.125 21.625v-3.75h-3.75v3.75zM29.125 27.25v-3.75h-3.75v3.75zM6.7 14.75l3.538 6.163h2.238v-9.825h-2.175v6.287l-3.412-6.287h-2.362v9.825h2.175zM29.75 2.875c0.005-0 0.010-0 0.015-0 0.339 0 0.645 0.144 0.859 0.374l0.001 0.001c0.231 0.215 0.375 0.52 0.375 0.86 0 0.005-0 0.011-0 0.016v-0.001 23.75c-0.017 0.683-0.567 1.233-1.248 1.25l-0.002 0h-20c-0.683-0.017-1.233-0.567-1.25-1.248l-0-0.002v-4.375h-6.25c-0.005 0-0.010 0-0.015 0-0.339 0-0.645-0.144-0.859-0.374l-0.001-0.001c-0.231-0.215-0.375-0.52-0.375-0.86 0-0.005 0-0.011 0-0.016v0.001-12.5c-0-0.005-0-0.010-0-0.015 0-0.339 0.144-0.645 0.374-0.859l0.001-0.001c0.211-0.231 0.513-0.375 0.849-0.375 0.009 0 0.018 0 0.028 0l-0.001-0h6.25v-4.375c-0-0.005-0-0.010-0-0.015 0-0.339 0.144-0.645 0.374-0.859l0.001-0.001c0.215-0.231 0.52-0.375 0.86-0.375 0.005 0 0.011 0 0.016 0h-0.001z"></path> </svg>)svg"},
        {"document_attach", R"svg(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 512 512" class="ionicon"><path d="M208 64h66.75a32 32 0 0 1 22.62 9.37l141.26 141.26a32 32 0 0 1 9.37 22.62V432a48 48 0 0 1-48 48H192a48 48 0 0 1-48-48V304" fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="32px"/><path d="M288 72v120a32 32 0 0 0 32 32h120" fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="32px"/><path d="M160 80v152a23.69 23.69 0 0 1-24 24c-12 0-24-9.1-24-24V88c0-30.59 16.57-56 48-56s48 24.8 48 55.38v138.75c0 43-27.82 77.87-72 77.87s-72-34.86-72-77.87V144" fill="none" stroke="currentColor" stroke-linecap="round" stroke-miterlimit="10" stroke-width="32px"/></svg>)svg"},
        {"star_001", R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16" fill="currentColor"><path d="M8 .25a.75.75 0 0 1 .673.418l1.882 3.815 4.21.612a.75.75 0 0 1 .416 1.279l-3.046 2.97.719 4.192a.751.751 0 0 1-1.088.791L8 12.347l-3.766 1.98a.75.75 0 0 1-1.088-.79l.72-4.194L.818 6.374a.75.75 0 0 1 .416-1.28l4.21-.611L7.327.668A.75.75 0 0 1 8 .25Z"/></svg>)svg"},
        {"star_002", R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16" fill="currentColor"><path d="M8 .25a.75.75 0 0 1 .673.418l1.882 3.815 4.21.612a.75.75 0 0 1 .416 1.279l-3.046 2.97.719 4.192a.751.751 0 0 1-1.088.791L8 12.347l-3.766 1.98a.75.75 0 0 1-1.088-.79l.72-4.194L.818 6.374a.75.75 0 0 1 .416-1.28l4.21-.611L7.327.668A.75.75 0 0 1 8 .25Zm0 2.445L6.615 5.5a.75.75 0 0 1-.564.41l-3.097.45 2.24 2.184a.75.75 0 0 1 .216.664l-.528 3.084 2.769-1.456a.75.75 0 0 1 .698 0l2.77 1.456-.53-3.084a.75.75 0 0 1 .216-.664l2.24-2.183-3.096-.45a.75.75 0 0 1-.564-.41L8 2.694Z"/></svg>)svg"},
        {"open", R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M18 13v6a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2V8a2 2 0 0 1 2-2h6"/><polyline points="15 3 21 3 21 9"/><line x1="10" y1="14" x2="21" y2="3"/></svg>)svg"},
        {"launch", R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M18 13v6a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2V8a2 2 0 0 1 2-2h6"/><polyline points="15 3 21 3 21 9"/><line x1="10" y1="14" x2="21" y2="3"/></svg>)svg"},
        {"folder_search", R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M22 19a2 2 0 0 1-2 2H4a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h5l2 3h9a2 2 0 0 1 2 2z"/><circle cx="11" cy="13" r="3"/><line x1="13.1" y1="15.1" x2="16" y2="18"/></svg>)svg"},
        {"paste", R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M16 4h2a2 2 0 0 1 2 2v14a2 2 0 0 1-2 2H6a2 2 0 0 1-2-2V6a2 2 0 0 1 2-2h2"/><rect x="8" y="2" width="8" height="4" rx="1" ry="1"/></svg>)svg"},
        {"more_horizontal", R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="1"/><circle cx="19" cy="12" r="1"/><circle cx="5" cy="12" r="1"/></svg>)svg"},
        {"sort", R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><line x1="12" y1="5" x2="12" y2="19"/><polyline points="19 12 12 19 5 12"/></svg>)svg"},
        {"paste_tag", R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M20.59 13.41l-7.17 7.17a2 2 0 0 1-2.83 0L2 12V2h10l8.59 8.59a2 2 0 0 1 0 2.82z"/><line x1="7" y1="7" x2="7.01" y2="7"/></svg>)svg"},
        {"repeat", R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polyline points="17 1 21 5 17 9"/><path d="M3 11V9a4 4 0 0 1 4-4h14"/><polyline points="7 23 3 19 7 15"/><path d="M21 13v2a4 4 0 0 1-4 4H3"/></svg>)svg"},
            {"airplay-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg"><path d="M22,4V16a1,1,0,0,1-1,1H17a1,1,0,0,1,0-2h3V5H4V15H7a1,1,0,0,1,0,2H3a1,1,0,0,1-1-1V4A1,1,0,0,1,3,3H21A1,1,0,0,1,22,4ZM12,15,8,21h8Z"/></svg>)svg"},
        {"airplay-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg"><path d="M22,4V16a1,1,0,0,1-1,1H17a1,1,0,0,1,0-2h3V5H4V15H7a1,1,0,0,1,0,2H3a1,1,0,0,1-1-1V4A1,1,0,0,1,3,3H21A1,1,0,0,1,22,4ZM12,15,8,21h8Z"/></svg>)svg"},
        {"airplay", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg"><path d="M22,4V16a1,1,0,0,1-1,1H17a1,1,0,0,1,0-2h3V5H4V15H7a1,1,0,0,1,0,2H3a1,1,0,0,1-1-1V4A1,1,0,0,1,3,3H21A1,1,0,0,1,22,4ZM12,15,8,21h8Z"/></svg>)svg"},
        {"alarm-bell-cancelled-filled-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="UTF-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg width="800px" height="800px" viewBox="0 0 512 512" version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
    <title>alarm-bell-cancelled-filled</title>
    <g id="Page-1" stroke="none" stroke-width="1" fill="none" fill-rule="evenodd">
        <g id="drop" fill="currentColor" transform="translate(59.581722, 42.666667)">
            <path d="M265.91044,362.667219 C260.734419,398.851421 229.615509,426.666667 192,426.666667 C154.384491,426.666667 123.265581,398.851421 118.08956,362.667219 Z M392.836561,379.581717 L362.666662,409.751616 L294.248,341.333 L4.418278,341.333333 L47.0849447,219.52 L47.0849447,170.666667 C47.0849447,147.564033 51.09328,125.537119 58.3600598,105.445633 L-2.84217094e-14,47.0849493 L30.169894,16.9150553 L78.758055,65.5041451 C78.7582845,65.5038101 78.7585139,65.5034752 78.7587433,65.5031403 L392.836561,379.581717 Z M196.418278,3.55271368e-14 C207.650729,0.712633806 218.754642,2.79014004 229.484945,6.18666667 C298.604945,21.9733333 345.751611,96 345.751611,176.853333 L345.751611,176.853333 L345.751611,219.52 L374.127,300.532 L107.279647,33.6843069 C131.533562,13.0442498 161.469156,0.623757746 193.946265,0.0228535467 Z" id="Combined-Shape">

</path>
        </g>
    </g>
</svg>)svg"},
        {"alarm-bell-cancelled-filled-svgrepo-com", R"svg(<?xml version="1.0" encoding="UTF-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg width="800px" height="800px" viewBox="0 0 512 512" version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
    <title>alarm-bell-cancelled-filled</title>
    <g id="Page-1" stroke="none" stroke-width="1" fill="none" fill-rule="evenodd">
        <g id="drop" fill="currentColor" transform="translate(59.581722, 42.666667)">
            <path d="M265.91044,362.667219 C260.734419,398.851421 229.615509,426.666667 192,426.666667 C154.384491,426.666667 123.265581,398.851421 118.08956,362.667219 Z M392.836561,379.581717 L362.666662,409.751616 L294.248,341.333 L4.418278,341.333333 L47.0849447,219.52 L47.0849447,170.666667 C47.0849447,147.564033 51.09328,125.537119 58.3600598,105.445633 L-2.84217094e-14,47.0849493 L30.169894,16.9150553 L78.758055,65.5041451 C78.7582845,65.5038101 78.7585139,65.5034752 78.7587433,65.5031403 L392.836561,379.581717 Z M196.418278,3.55271368e-14 C207.650729,0.712633806 218.754642,2.79014004 229.484945,6.18666667 C298.604945,21.9733333 345.751611,96 345.751611,176.853333 L345.751611,176.853333 L345.751611,219.52 L374.127,300.532 L107.279647,33.6843069 C131.533562,13.0442498 161.469156,0.623757746 193.946265,0.0228535467 Z" id="Combined-Shape">

</path>
        </g>
    </g>
</svg>)svg"},
        {"alarm_bell_cancelled_filled", R"svg(<?xml version="1.0" encoding="UTF-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg width="800px" height="800px" viewBox="0 0 512 512" version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
    <title>alarm-bell-cancelled-filled</title>
    <g id="Page-1" stroke="none" stroke-width="1" fill="none" fill-rule="evenodd">
        <g id="drop" fill="currentColor" transform="translate(59.581722, 42.666667)">
            <path d="M265.91044,362.667219 C260.734419,398.851421 229.615509,426.666667 192,426.666667 C154.384491,426.666667 123.265581,398.851421 118.08956,362.667219 Z M392.836561,379.581717 L362.666662,409.751616 L294.248,341.333 L4.418278,341.333333 L47.0849447,219.52 L47.0849447,170.666667 C47.0849447,147.564033 51.09328,125.537119 58.3600598,105.445633 L-2.84217094e-14,47.0849493 L30.169894,16.9150553 L78.758055,65.5041451 C78.7582845,65.5038101 78.7585139,65.5034752 78.7587433,65.5031403 L392.836561,379.581717 Z M196.418278,3.55271368e-14 C207.650729,0.712633806 218.754642,2.79014004 229.484945,6.18666667 C298.604945,21.9733333 345.751611,96 345.751611,176.853333 L345.751611,176.853333 L345.751611,219.52 L374.127,300.532 L107.279647,33.6843069 C131.533562,13.0442498 161.469156,0.623757746 193.946265,0.0228535467 Z" id="Combined-Shape">

</path>
        </g>
    </g>
</svg>)svg"},
        {"alarm-clock-filled-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="UTF-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg width="800px" height="800px" viewBox="0 0 512 512" version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
    <title>alarm-clock-filled</title>
    <g id="Page-1" stroke="none" stroke-width="1" fill="none" fill-rule="evenodd">
        <g id="icon" fill="currentColor" transform="translate(42.986146, 42.979833)">
            <path d="M213.013854,21.020167 C319.052526,21.020167 405.013854,106.981495 405.013854,213.020167 C405.013854,265.317935 384.104488,312.732059 350.189566,347.358729 L383.321027,404.745351 L346.37061,426.078684 L316.681804,374.65549 C286.780409,393.873084 251.198902,405.020167 213.013854,405.020167 C174.771065,405.020167 139.139696,393.839346 109.210297,374.568255 L79.3135646,426.351941 L42.3631474,405.018608 L75.7203109,347.23832 C41.874796,312.622182 21.0138542,265.257342 21.0138542,213.020167 C21.0138542,106.981495 106.975182,21.020167 213.013854,21.020167 Z M234.347187,106.3535 L191.680521,106.3535 L191.680521,243.190056 L261.928909,313.438445 L292.098799,283.268556 L234.347187,225.500167 L234.347187,106.3535 Z M355.032367,7.10542736e-15 C383.117284,18.7598588 407.283905,42.9259293 426.044403,71.0103829 L390.537168,94.6761076 C374.907194,71.2792434 354.774678,51.1459711 331.378448,35.5151218 L355.032367,7.10542736e-15 Z M70.9851521,0.01209835 L94.6571811,35.5151218 C71.2632279,51.1444505 51.1323522,71.2753263 35.5030235,94.6692795 L1.20792265e-13,70.9972504 C18.7552254,42.9244324 42.912334,18.7673237 70.9851521,0.01209835 Z" id="Combined-Shape">

</path>
        </g>
    </g>
</svg>)svg"},
        {"alarm-clock-filled-svgrepo-com", R"svg(<?xml version="1.0" encoding="UTF-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg width="800px" height="800px" viewBox="0 0 512 512" version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
    <title>alarm-clock-filled</title>
    <g id="Page-1" stroke="none" stroke-width="1" fill="none" fill-rule="evenodd">
        <g id="icon" fill="currentColor" transform="translate(42.986146, 42.979833)">
            <path d="M213.013854,21.020167 C319.052526,21.020167 405.013854,106.981495 405.013854,213.020167 C405.013854,265.317935 384.104488,312.732059 350.189566,347.358729 L383.321027,404.745351 L346.37061,426.078684 L316.681804,374.65549 C286.780409,393.873084 251.198902,405.020167 213.013854,405.020167 C174.771065,405.020167 139.139696,393.839346 109.210297,374.568255 L79.3135646,426.351941 L42.3631474,405.018608 L75.7203109,347.23832 C41.874796,312.622182 21.0138542,265.257342 21.0138542,213.020167 C21.0138542,106.981495 106.975182,21.020167 213.013854,21.020167 Z M234.347187,106.3535 L191.680521,106.3535 L191.680521,243.190056 L261.928909,313.438445 L292.098799,283.268556 L234.347187,225.500167 L234.347187,106.3535 Z M355.032367,7.10542736e-15 C383.117284,18.7598588 407.283905,42.9259293 426.044403,71.0103829 L390.537168,94.6761076 C374.907194,71.2792434 354.774678,51.1459711 331.378448,35.5151218 L355.032367,7.10542736e-15 Z M70.9851521,0.01209835 L94.6571811,35.5151218 C71.2632279,51.1444505 51.1323522,71.2753263 35.5030235,94.6692795 L1.20792265e-13,70.9972504 C18.7552254,42.9244324 42.912334,18.7673237 70.9851521,0.01209835 Z" id="Combined-Shape">

</path>
        </g>
    </g>
</svg>)svg"},
        {"alarm_clock_filled", R"svg(<?xml version="1.0" encoding="UTF-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg width="800px" height="800px" viewBox="0 0 512 512" version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
    <title>alarm-clock-filled</title>
    <g id="Page-1" stroke="none" stroke-width="1" fill="none" fill-rule="evenodd">
        <g id="icon" fill="currentColor" transform="translate(42.986146, 42.979833)">
            <path d="M213.013854,21.020167 C319.052526,21.020167 405.013854,106.981495 405.013854,213.020167 C405.013854,265.317935 384.104488,312.732059 350.189566,347.358729 L383.321027,404.745351 L346.37061,426.078684 L316.681804,374.65549 C286.780409,393.873084 251.198902,405.020167 213.013854,405.020167 C174.771065,405.020167 139.139696,393.839346 109.210297,374.568255 L79.3135646,426.351941 L42.3631474,405.018608 L75.7203109,347.23832 C41.874796,312.622182 21.0138542,265.257342 21.0138542,213.020167 C21.0138542,106.981495 106.975182,21.020167 213.013854,21.020167 Z M234.347187,106.3535 L191.680521,106.3535 L191.680521,243.190056 L261.928909,313.438445 L292.098799,283.268556 L234.347187,225.500167 L234.347187,106.3535 Z M355.032367,7.10542736e-15 C383.117284,18.7598588 407.283905,42.9259293 426.044403,71.0103829 L390.537168,94.6761076 C374.907194,71.2792434 354.774678,51.1459711 331.378448,35.5151218 L355.032367,7.10542736e-15 Z M70.9851521,0.01209835 L94.6571811,35.5151218 C71.2632279,51.1444505 51.1323522,71.2753263 35.5030235,94.6692795 L1.20792265e-13,70.9972504 C18.7552254,42.9244324 42.912334,18.7673237 70.9851521,0.01209835 Z" id="Combined-Shape">

</path>
        </g>
    </g>
</svg>)svg"},
        {"bag-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg"><path d="M5,21H19a2.006,2.006,0,0,0,2-2V9a2.006,2.006,0,0,0-2-2H17V5a2,2,0,0,0-2-2H9A2,2,0,0,0,7,5V7H5A2.006,2.006,0,0,0,3,9V19A2.006,2.006,0,0,0,5,21ZM16,9h2V19H16ZM9,5.5A.5.5,0,0,1,9.5,5h5a.5.5,0,0,1,.5.5V7H9ZM6,9H8V19H6Z"/></svg>)svg"},
        {"bag-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg"><path d="M5,21H19a2.006,2.006,0,0,0,2-2V9a2.006,2.006,0,0,0-2-2H17V5a2,2,0,0,0-2-2H9A2,2,0,0,0,7,5V7H5A2.006,2.006,0,0,0,3,9V19A2.006,2.006,0,0,0,5,21ZM16,9h2V19H16ZM9,5.5A.5.5,0,0,1,9.5,5h5a.5.5,0,0,1,.5.5V7H9ZM6,9H8V19H6Z"/></svg>)svg"},
        {"bag", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg"><path d="M5,21H19a2.006,2.006,0,0,0,2-2V9a2.006,2.006,0,0,0-2-2H17V5a2,2,0,0,0-2-2H9A2,2,0,0,0,7,5V7H5A2.006,2.006,0,0,0,3,9V19A2.006,2.006,0,0,0,5,21ZM16,9h2V19H16ZM9,5.5A.5.5,0,0,1,9.5,5h5a.5.5,0,0,1,.5.5V7H9ZM6,9H8V19H6Z"/></svg>)svg"},
        {"book2-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>

<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg width="800px" height="800px" viewBox="0 0 48 48" xmlns="http://www.w3.org/2000/svg" >

<path d="M0 0h48v48H0z" fill="none"/>
<g id="Shopicon">
	<path d="M4,38h14c2.206,0,4,1.794,4,4v2h4v-2c0-2.206,1.794-4,4-4h14V4H30c-2.39,0-4.533,1.059-6,2.726C22.533,5.059,20.39,4,18,4
		H4V38z M30,8h10v26H30c-1.458,0-2.822,0.398-4,1.082V12C26,9.794,27.794,8,30,8z M8,8h10c2.206,0,4,1.794,4,4v23.082
		C20.822,34.398,19.458,34,18,34H8V8z"/>
</g>
</svg>)svg"},
        {"book2-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?>

<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg width="800px" height="800px" viewBox="0 0 48 48" xmlns="http://www.w3.org/2000/svg" >

<path d="M0 0h48v48H0z" fill="none"/>
<g id="Shopicon">
	<path d="M4,38h14c2.206,0,4,1.794,4,4v2h4v-2c0-2.206,1.794-4,4-4h14V4H30c-2.39,0-4.533,1.059-6,2.726C22.533,5.059,20.39,4,18,4
		H4V38z M30,8h10v26H30c-1.458,0-2.822,0.398-4,1.082V12C26,9.794,27.794,8,30,8z M8,8h10c2.206,0,4,1.794,4,4v23.082
		C20.822,34.398,19.458,34,18,34H8V8z"/>
</g>
</svg>)svg"},
        {"book2", R"svg(<?xml version="1.0" encoding="utf-8"?>

<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg width="800px" height="800px" viewBox="0 0 48 48" xmlns="http://www.w3.org/2000/svg" >

<path d="M0 0h48v48H0z" fill="none"/>
<g id="Shopicon">
	<path d="M4,38h14c2.206,0,4,1.794,4,4v2h4v-2c0-2.206,1.794-4,4-4h14V4H30c-2.39,0-4.533,1.059-6,2.726C22.533,5.059,20.39,4,18,4
		H4V38z M30,8h10v26H30c-1.458,0-2.822,0.398-4,1.082V12C26,9.794,27.794,8,30,8z M8,8h10c2.206,0,4,1.794,4,4v23.082
		C20.822,34.398,19.458,34,18,34H8V8z"/>
</g>
</svg>)svg"},
        {"bookmark-book-svgrepo-com (1).svg", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg"><path d="M6,22H19a1,1,0,0,0,1-1V3a1,1,0,0,0-1-1H6A2,2,0,0,0,4,4V20A2,2,0,0,0,6,22ZM7,4h5v8L9.5,10,7,12Z"/></svg>)svg"},
        {"bookmark-book-svgrepo-com (1)", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg"><path d="M6,22H19a1,1,0,0,0,1-1V3a1,1,0,0,0-1-1H6A2,2,0,0,0,4,4V20A2,2,0,0,0,6,22ZM7,4h5v8L9.5,10,7,12Z"/></svg>)svg"},
        {"bookmark_book", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg"><path d="M6,22H19a1,1,0,0,0,1-1V3a1,1,0,0,0-1-1H6A2,2,0,0,0,4,4V20A2,2,0,0,0,6,22ZM7,4h5v8L9.5,10,7,12Z"/></svg>)svg"},
        {"bookmark-book-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg"><path d="M6,22H19a1,1,0,0,0,1-1V3a1,1,0,0,0-1-1H6A2,2,0,0,0,4,4V20A2,2,0,0,0,6,22ZM7,4h5v8L9.5,10,7,12Z"/></svg>)svg"},
        {"bookmark-book-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg"><path d="M6,22H19a1,1,0,0,0,1-1V3a1,1,0,0,0-1-1H6A2,2,0,0,0,4,4V20A2,2,0,0,0,6,22ZM7,4h5v8L9.5,10,7,12Z"/></svg>)svg"},
        {"bookmark1-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>bookmark1</title>
<path d="M25 29h-16v-22h7v9.125l3.541-2.688 3.459 2.688v-9.125h4v20c0 1.104-0.896 2-2 2zM19.541 12.125l-2.541 2v-8.125h5v8.125l-2.459-2zM6 5v1c0 0.552 0.448 1 1 1h1v22h-1c-1.104 0-2-0.896-2-2v-22c0-1.104 0.896-2 2-2h18c0.738 0 1.376 0.404 1.723 1h-19.723c-0.552 0-1 0.447-1 1z"></path>
</svg>)svg"},
        {"bookmark1-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>bookmark1</title>
<path d="M25 29h-16v-22h7v9.125l3.541-2.688 3.459 2.688v-9.125h4v20c0 1.104-0.896 2-2 2zM19.541 12.125l-2.541 2v-8.125h5v8.125l-2.459-2zM6 5v1c0 0.552 0.448 1 1 1h1v22h-1c-1.104 0-2-0.896-2-2v-22c0-1.104 0.896-2 2-2h18c0.738 0 1.376 0.404 1.723 1h-19.723c-0.552 0-1 0.447-1 1z"></path>
</svg>)svg"},
        {"bookmark1", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>bookmark1</title>
<path d="M25 29h-16v-22h7v9.125l3.541-2.688 3.459 2.688v-9.125h4v20c0 1.104-0.896 2-2 2zM19.541 12.125l-2.541 2v-8.125h5v8.125l-2.459-2zM6 5v1c0 0.552 0.448 1 1 1h1v22h-1c-1.104 0-2-0.896-2-2v-22c0-1.104 0.896-2 2-2h18c0.738 0 1.376 0.404 1.723 1h-19.723c-0.552 0-1 0.447-1 1z"></path>
</svg>)svg"},
        {"circle-left-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg width="800px" height="800px" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg">
<path d="M14 11L10 15L14 19" stroke="currentColor" stroke-width="2"/>
<path d="M4.20577 12.75C3.21517 11.8921 2.8184 10.8948 3.077 9.91263C3.3356 8.9305 4.23511 8.01848 5.63604 7.31802C7.03696 6.61756 8.86101 6.1678 10.8253 6.0385C12.7895 5.9092 14.7842 6.10758 16.5 6.60289C18.2158 7.09819 19.5567 7.86273 20.3149 8.77792C21.0731 9.69312 21.2061 10.7078 20.6933 11.6647C20.1806 12.6215 19.0507 13.467 17.4789 14.0701C15.9071 14.6731 13.9812 15 12 15" stroke="currentColor" stroke-width="2" stroke-linecap="round"/>
</svg>)svg"},
        {"circle-left-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg width="800px" height="800px" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg">
<path d="M14 11L10 15L14 19" stroke="currentColor" stroke-width="2"/>
<path d="M4.20577 12.75C3.21517 11.8921 2.8184 10.8948 3.077 9.91263C3.3356 8.9305 4.23511 8.01848 5.63604 7.31802C7.03696 6.61756 8.86101 6.1678 10.8253 6.0385C12.7895 5.9092 14.7842 6.10758 16.5 6.60289C18.2158 7.09819 19.5567 7.86273 20.3149 8.77792C21.0731 9.69312 21.2061 10.7078 20.6933 11.6647C20.1806 12.6215 19.0507 13.467 17.4789 14.0701C15.9071 14.6731 13.9812 15 12 15" stroke="currentColor" stroke-width="2" stroke-linecap="round"/>
</svg>)svg"},
        {"circle_left", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg width="800px" height="800px" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg">
<path d="M14 11L10 15L14 19" stroke="currentColor" stroke-width="2"/>
<path d="M4.20577 12.75C3.21517 11.8921 2.8184 10.8948 3.077 9.91263C3.3356 8.9305 4.23511 8.01848 5.63604 7.31802C7.03696 6.61756 8.86101 6.1678 10.8253 6.0385C12.7895 5.9092 14.7842 6.10758 16.5 6.60289C18.2158 7.09819 19.5567 7.86273 20.3149 8.77792C21.0731 9.69312 21.2061 10.7078 20.6933 11.6647C20.1806 12.6215 19.0507 13.467 17.4789 14.0701C15.9071 14.6731 13.9812 15 12 15" stroke="currentColor" stroke-width="2" stroke-linecap="round"/>
</svg>)svg"},
        {"circle-right-alt-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg width="800px" height="800px" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg">
<path d="M10 11L14 15L10 19" stroke="currentColor" stroke-width="2"/>
<path d="M9.67063 6.15333C7.56156 6.4359 5.72985 7.09219 4.51677 7.99993C3.3037 8.90768 2.792 10.005 3.077 11.0874C3.362 12.1698 4.42426 13.1634 6.06589 13.8833C7.70751 14.6031 9.81652 15 12 15" stroke="currentColor" stroke-width="2" stroke-linecap="round"/>
<path d="M19.7942 12.75C20.3852 12.2382 20.7687 11.6733 20.923 11.0874C21.0773 10.5015 20.9992 9.90613 20.6933 9.33531C20.3874 8.7645 19.8597 8.2294 19.1402 7.76057C18.4207 7.29174 17.5236 6.89836 16.5 6.60289" stroke="currentColor" stroke-width="2" stroke-linecap="round"/>
</svg>)svg"},
        {"circle-right-alt-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg width="800px" height="800px" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg">
<path d="M10 11L14 15L10 19" stroke="currentColor" stroke-width="2"/>
<path d="M9.67063 6.15333C7.56156 6.4359 5.72985 7.09219 4.51677 7.99993C3.3037 8.90768 2.792 10.005 3.077 11.0874C3.362 12.1698 4.42426 13.1634 6.06589 13.8833C7.70751 14.6031 9.81652 15 12 15" stroke="currentColor" stroke-width="2" stroke-linecap="round"/>
<path d="M19.7942 12.75C20.3852 12.2382 20.7687 11.6733 20.923 11.0874C21.0773 10.5015 20.9992 9.90613 20.6933 9.33531C20.3874 8.7645 19.8597 8.2294 19.1402 7.76057C18.4207 7.29174 17.5236 6.89836 16.5 6.60289" stroke="currentColor" stroke-width="2" stroke-linecap="round"/>
</svg>)svg"},
        {"circle_right_alt", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg width="800px" height="800px" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg">
<path d="M10 11L14 15L10 19" stroke="currentColor" stroke-width="2"/>
<path d="M9.67063 6.15333C7.56156 6.4359 5.72985 7.09219 4.51677 7.99993C3.3037 8.90768 2.792 10.005 3.077 11.0874C3.362 12.1698 4.42426 13.1634 6.06589 13.8833C7.70751 14.6031 9.81652 15 12 15" stroke="currentColor" stroke-width="2" stroke-linecap="round"/>
<path d="M19.7942 12.75C20.3852 12.2382 20.7687 11.6733 20.923 11.0874C21.0773 10.5015 20.9992 9.90613 20.6933 9.33531C20.3874 8.7645 19.8597 8.2294 19.1402 7.76057C18.4207 7.29174 17.5236 6.89836 16.5 6.60289" stroke="currentColor" stroke-width="2" stroke-linecap="round"/>
</svg>)svg"},
        {"circle-right-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg width="800px" height="800px" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg">
<path d="M10 11L14 15L10 19" stroke="currentColor" stroke-width="2"/>
<path d="M19.7942 12.75C20.7848 11.8921 21.1816 10.8948 20.923 9.91263C20.6644 8.9305 19.7649 8.01848 18.364 7.31802C16.963 6.61756 15.139 6.1678 13.1747 6.0385C11.2105 5.9092 9.21578 6.10758 7.5 6.60289C5.78422 7.09819 4.44326 7.86273 3.68508 8.77792C2.92691 9.69312 2.79389 10.7078 3.30667 11.6647C3.81944 12.6215 4.94935 13.467 6.52115 14.0701C8.09295 14.6731 10.0188 15 12 15" stroke="currentColor" stroke-width="2" stroke-linecap="round"/>
</svg>)svg"},
        {"circle-right-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg width="800px" height="800px" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg">
<path d="M10 11L14 15L10 19" stroke="currentColor" stroke-width="2"/>
<path d="M19.7942 12.75C20.7848 11.8921 21.1816 10.8948 20.923 9.91263C20.6644 8.9305 19.7649 8.01848 18.364 7.31802C16.963 6.61756 15.139 6.1678 13.1747 6.0385C11.2105 5.9092 9.21578 6.10758 7.5 6.60289C5.78422 7.09819 4.44326 7.86273 3.68508 8.77792C2.92691 9.69312 2.79389 10.7078 3.30667 11.6647C3.81944 12.6215 4.94935 13.467 6.52115 14.0701C8.09295 14.6731 10.0188 15 12 15" stroke="currentColor" stroke-width="2" stroke-linecap="round"/>
</svg>)svg"},
        {"circle_right", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg width="800px" height="800px" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg">
<path d="M10 11L14 15L10 19" stroke="currentColor" stroke-width="2"/>
<path d="M19.7942 12.75C20.7848 11.8921 21.1816 10.8948 20.923 9.91263C20.6644 8.9305 19.7649 8.01848 18.364 7.31802C16.963 6.61756 15.139 6.1678 13.1747 6.0385C11.2105 5.9092 9.21578 6.10758 7.5 6.60289C5.78422 7.09819 4.44326 7.86273 3.68508 8.77792C2.92691 9.69312 2.79389 10.7078 3.30667 11.6647C3.81944 12.6215 4.94935 13.467 6.52115 14.0701C8.09295 14.6731 10.0188 15 12 15" stroke="currentColor" stroke-width="2" stroke-linecap="round"/>
</svg>)svg"},
        {"close-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg width="800px" height="800px" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg"><path fill-rule="evenodd" clip-rule="evenodd" d="M19.207 6.207a1 1 0 0 0-1.414-1.414L12 10.586 6.207 4.793a1 1 0 0 0-1.414 1.414L10.586 12l-5.793 5.793a1 1 0 1 0 1.414 1.414L12 13.414l5.793 5.793a1 1 0 0 0 1.414-1.414L13.414 12l5.793-5.793z" fill="currentColor"/></svg>)svg"},
        {"close-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg width="800px" height="800px" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg"><path fill-rule="evenodd" clip-rule="evenodd" d="M19.207 6.207a1 1 0 0 0-1.414-1.414L12 10.586 6.207 4.793a1 1 0 0 0-1.414 1.414L10.586 12l-5.793 5.793a1 1 0 1 0 1.414 1.414L12 13.414l5.793 5.793a1 1 0 0 0 1.414-1.414L13.414 12l5.793-5.793z" fill="currentColor"/></svg>)svg"},
        {"corel-draw-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor"  version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink"  width="800px"
	 height="800px" viewBox="0 0 512 512" enable-background="new 0 0 512 512" xml:space="preserve">

<g id="2069a460dcf28295e231f3111e0396ed">

<path display="inline" d="M239.532,463.443h32.923c5.977,0,10.85,4.873,10.85,10.85v26.369c0,5.963-4.873,10.837-10.85,10.837
		h-32.923c-5.963,0-10.837-4.874-10.837-10.837v-26.369C228.695,468.316,233.569,463.443,239.532,463.443L239.532,463.443z
		 M239.532,463.443 M102.525,146.023c0.412,81.391,69.535,208.35,133.431,295.97h6.288
		c-48.593-88.639-90.036-197.933-90.402-284.073c-0.341-78.151,31.58-127.745,74.662-156.048c-9.564,0.89-19.146,2.217-28.64,4.042
		C144.734,27.04,102.144,70.638,102.525,146.023L102.525,146.023z M102.525,146.023 M243.163,0.753
		c-24.897,33.505-44.102,81.012-43.931,149.948c0.229,89.978,21.279,196.93,48.426,291.292h16.684
		c27.147-94.361,48.197-201.314,48.427-291.292c0.17-68.936-19.034-116.443-43.931-149.948
		C260.288,0.417,251.713,0.417,243.163,0.753L243.163,0.753z M243.163,0.753 M409.475,146.023
		c-0.411,81.391-69.534,208.35-133.431,295.97h-6.287c48.592-88.639,90.036-197.933,90.397-284.073
		C360.5,79.77,328.579,30.175,285.497,1.872c9.564,0.89,19.146,2.217,28.627,4.042C367.266,27.04,409.854,70.638,409.475,146.023
		L409.475,146.023z M409.475,146.023 M280.382,441.993c71.526-75.27,152.676-201.084,153.212-298.033
		c0.325-59.454-28.614-98.881-69.151-122.959c49.665,21.308,87.449,60.635,87.083,124.556
		c-0.582,98.475-94.968,228.569-169.9,296.436H280.382z M280.382,441.993 M231.619,441.993
		C160.092,366.724,78.943,240.909,78.406,143.96c-0.324-59.454,28.603-98.881,69.14-122.959
		C97.894,42.31,60.109,81.637,60.475,145.557c0.583,98.475,94.969,228.569,169.9,296.436H231.619z M231.619,441.993">

</path>

</g>

</svg>)svg"},
        {"corel-draw-svgrepo-com", R"svg(<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor"  version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink"  width="800px"
	 height="800px" viewBox="0 0 512 512" enable-background="new 0 0 512 512" xml:space="preserve">

<g id="2069a460dcf28295e231f3111e0396ed">

<path display="inline" d="M239.532,463.443h32.923c5.977,0,10.85,4.873,10.85,10.85v26.369c0,5.963-4.873,10.837-10.85,10.837
		h-32.923c-5.963,0-10.837-4.874-10.837-10.837v-26.369C228.695,468.316,233.569,463.443,239.532,463.443L239.532,463.443z
		 M239.532,463.443 M102.525,146.023c0.412,81.391,69.535,208.35,133.431,295.97h6.288
		c-48.593-88.639-90.036-197.933-90.402-284.073c-0.341-78.151,31.58-127.745,74.662-156.048c-9.564,0.89-19.146,2.217-28.64,4.042
		C144.734,27.04,102.144,70.638,102.525,146.023L102.525,146.023z M102.525,146.023 M243.163,0.753
		c-24.897,33.505-44.102,81.012-43.931,149.948c0.229,89.978,21.279,196.93,48.426,291.292h16.684
		c27.147-94.361,48.197-201.314,48.427-291.292c0.17-68.936-19.034-116.443-43.931-149.948
		C260.288,0.417,251.713,0.417,243.163,0.753L243.163,0.753z M243.163,0.753 M409.475,146.023
		c-0.411,81.391-69.534,208.35-133.431,295.97h-6.287c48.592-88.639,90.036-197.933,90.397-284.073
		C360.5,79.77,328.579,30.175,285.497,1.872c9.564,0.89,19.146,2.217,28.627,4.042C367.266,27.04,409.854,70.638,409.475,146.023
		L409.475,146.023z M409.475,146.023 M280.382,441.993c71.526-75.27,152.676-201.084,153.212-298.033
		c0.325-59.454-28.614-98.881-69.151-122.959c49.665,21.308,87.449,60.635,87.083,124.556
		c-0.582,98.475-94.968,228.569-169.9,296.436H280.382z M280.382,441.993 M231.619,441.993
		C160.092,366.724,78.943,240.909,78.406,143.96c-0.324-59.454,28.603-98.881,69.14-122.959
		C97.894,42.31,60.109,81.637,60.475,145.557c0.583,98.475,94.969,228.569,169.9,296.436H231.619z M231.619,441.993">

</path>

</g>

</svg>)svg"},
        {"corel_draw", R"svg(<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor"  version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink"  width="800px"
	 height="800px" viewBox="0 0 512 512" enable-background="new 0 0 512 512" xml:space="preserve">

<g id="2069a460dcf28295e231f3111e0396ed">

<path display="inline" d="M239.532,463.443h32.923c5.977,0,10.85,4.873,10.85,10.85v26.369c0,5.963-4.873,10.837-10.85,10.837
		h-32.923c-5.963,0-10.837-4.874-10.837-10.837v-26.369C228.695,468.316,233.569,463.443,239.532,463.443L239.532,463.443z
		 M239.532,463.443 M102.525,146.023c0.412,81.391,69.535,208.35,133.431,295.97h6.288
		c-48.593-88.639-90.036-197.933-90.402-284.073c-0.341-78.151,31.58-127.745,74.662-156.048c-9.564,0.89-19.146,2.217-28.64,4.042
		C144.734,27.04,102.144,70.638,102.525,146.023L102.525,146.023z M102.525,146.023 M243.163,0.753
		c-24.897,33.505-44.102,81.012-43.931,149.948c0.229,89.978,21.279,196.93,48.426,291.292h16.684
		c27.147-94.361,48.197-201.314,48.427-291.292c0.17-68.936-19.034-116.443-43.931-149.948
		C260.288,0.417,251.713,0.417,243.163,0.753L243.163,0.753z M243.163,0.753 M409.475,146.023
		c-0.411,81.391-69.534,208.35-133.431,295.97h-6.287c48.592-88.639,90.036-197.933,90.397-284.073
		C360.5,79.77,328.579,30.175,285.497,1.872c9.564,0.89,19.146,2.217,28.627,4.042C367.266,27.04,409.854,70.638,409.475,146.023
		L409.475,146.023z M409.475,146.023 M280.382,441.993c71.526-75.27,152.676-201.084,153.212-298.033
		c0.325-59.454-28.614-98.881-69.151-122.959c49.665,21.308,87.449,60.635,87.083,124.556
		c-0.582,98.475-94.968,228.569-169.9,296.436H280.382z M280.382,441.993 M231.619,441.993
		C160.092,366.724,78.943,240.909,78.406,143.96c-0.324-59.454,28.603-98.881,69.14-122.959
		C97.894,42.31,60.109,81.637,60.475,145.557c0.583,98.475,94.969,228.569,169.9,296.436H231.619z M231.619,441.993">

</path>

</g>

</svg>)svg"},
        {"dbs-hadoop-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor"  version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink"  width="800px"
	 height="800px" viewBox="0 0 512 512" enable-background="new 0 0 512 512" xml:space="preserve">

<g id="3e91140ac1bfb9903b91c1b0ca0929ce">

<path display="inline" d="M482.007,132.576c-3.451,13.652-10.421,25.263-24.859,30.405c-7.078,2.52-11.652-0.073-18.871-2.154
		c7.086,0.674,11.161,1.763,17.507-1.378C467.631,153.591,476.36,144.019,482.007,132.576z M510.854,222.319
		c-1.684,16.547-6.101,32.622-13.968,46.247c-7.045,12.218-16.863,22.489-29.941,29.492c-8.887,4.725-19.213,7.461-29.888,8.579
		c-11.082,1.152-22.522,0.536-33.197-1.276c-0.499,1.654-1.015,3.335-1.506,4.978c-0.624,2.066-1.256,4.146-1.88,6.2
		c-2.179,7.161-7.385,13.615-13.872,18.397c-6.695,4.94-14.858,8.167-22.469,8.509c-7.27,0.353-12.888-1.36-17.869-4.301
		c-4.824-2.836-8.949-6.782-13.449-11.128l-5.896-5.672c-1.322-1.289-2.645-2.574-3.983-3.851c-0.142,0.927-0.274,1.813-0.416,2.623
		c2.079,4.205,3.551,8.147,5.048,12.189v0.012c0.944,2.524,1.913,5.119,3.082,7.931c1.742,4.158,3.057,7.627,3.972,11.224
		c0.914,3.631,1.418,7.323,1.53,11.848c0.107,3.855,0.212,7.702,0.319,11.549c0.096,3.63,0.188,7.261,0.304,10.891
		c1.605,2.911,2.554,4.907,3.027,7.111c0.532,2.35,0.479,4.641,0.208,8.375c-0.59,8.393-3.364,13.07-8.025,15.724
		c-4.375,2.479-10.168,2.849-17.333,2.815c-1.09-0.017-2.856-0.017-4.999-0.017c-6.952,0-17.706,0.017-20.996-0.216
		c-4.42-0.32-8.097-0.657-11.182-1.397c-2.379-0.565-4.334-1.39-5.968-2.587c-2.375,1.917-4.462,3.651-6.559,5.377
		c-6.173,5.086-12.439,10.268-14.963,11.947c-0.967,1.489-1.842,2.99-2.645,4.384c-2.591,4.541-4.508,7.9-10.617,11.215
		c-11.998,6.479-21.283,5.914-29.428,0.915c-7.76-4.762-14.17-13.511-20.872-23.849c-3.188-4.907-8.278-12.991-10.756-20.918
		c-2.74-8.774-2.36-17.308,6.381-21.799c0.416-0.216,0.838-0.433,1.233-0.641c0.911-0.474,1.77-0.906,2.628-1.348
		c-3.313,0.175-6.643,0.309-9.956,0.267c-6.504-0.042-13.039-0.49-19.703-1.348c0.108,0.795,0.2,1.431,0.251,1.843
		c0.51,4.42,0.863,7.314,0.443,10.213c-0.422,2.99-1.609,5.651-4.136,9.419c-0.168,0.25-0.734,1.069-1.218,1.755
		c-2.343,3.452-3.728,5.506-6.951,12.638c0.318,3.515,0.422,6.342,0.189,9.157c-0.256,3.032-0.894,6.117-1.973,10.151
		c-2.751,10.172-12.979,12.517-24.311,11.806c-10.398-0.648-21.782-3.921-28.359-5.709c-1.428-0.379-2.586-0.679-3.711-0.974
		c-5.55-1.447-11.53-3.011-16.563-5.298c-10.078-4.6-16.537-11.818-10.247-25.638c1.013-2.191,1.896-4.42,2.699-6.666
		c0.308-0.798,0.51-1.563,0.771-2.361c-5.131,0.798-10.582-0.175-15.89-2.192c-9.286-3.53-18.16-10.275-23.589-15.706
		c-4.341-4.351-7.718-9.032-10.245-13.84c-2.734-5.207-4.483-10.604-5.373-15.919c-1.466-8.699-0.443-9.689,4.699-14.626
		c0.767-0.752,1.626-1.571,1.836-1.775c2.541-2.512,5.065-5.031,7.6-7.531c2.705-2.682,5.441-5.385,8.136-8.08
		c1.239-3.352,2.472-6.695,3.715-10.076c0.799-2.158,1.591-4.333,2.381-6.482c-0.38-2.005-0.724-4.021-1.013-6.084
		c-0.334-2.408-0.597-4.62-0.771-6.616c-0.782-9.194-1.044-17.101-0.771-24.261c0.206-5.461,0.715-10.484,1.553-15.291
		c-4.246,0.946-8.571,0.873-12.49-0.37c-4.843-1.564-9.07-4.907-11.688-10.403c-0.981-2.017-1.753-4.057-2.483-6.36
		c-0.642-2.032-1.235-4.3-1.871-6.87c-3.473-3.578-5.441-7.333-6.094-11.178c-0.776-4.413,0.118-8.814,2.094-13.031
		c1.892-4.053,4.797-7.908,8.159-11.405c9.055-9.405,21.926-16.474,27.916-17.514l4.493-0.786l-1.495,4.304
		c-0.777,2.179-1.686,4.462-2.565,6.709c-0.248,0.653-0.499,1.312-0.771,2.021c2.755,2.179,4.406,5.202,5.177,8.643
		c1.204,5.321,0.426,11.451-0.601,16.038l-1.225,5.491l-3.48-4.402c-0.842-1.038-1.543-2.119-2.217-3.167
		c-0.622-0.957-1.212-1.857-1.815-2.597c-0.479,7.143-1.348,14.306-6.438,19.189c0.144,0.614,0.212,0.942,0.314,1.013
		c0.081,0.042,0.946-0.43,3.034-1.243c3.584-1.368,6.772-3.651,9.771-6.335c3.083-2.776,5.903-5.945,8.756-9.081
		c3.225-6.263,6.916-12.187,11.226-17.821c4.381-5.749,9.395-11.184,15.183-16.312c14.166-12.546,27.947-20.901,43.59-26.954
		c15.312-5.917,32.382-9.646,53.352-13.008c2.639-2.736,5.317-5.446,8.063-8.126c2.946-2.828,5.99-5.656,9.178-8.483
		c4.479-3.938,8.222-6.346,11.945-7.836c3.362-1.368,6.679-1.903,10.407-2.175c6.402-8.535,12.239-15.858,19.106-22.202
		c7.19-6.649,15.447-12.183,26.564-16.857c23.025-9.712,42.546-14.771,61.01-13.611c18.597,1.163,35.996,8.608,54.643,23.993
		c3.519,2.907,7.103,6.047,10.675,9.184c7.814,6.87,15.482,13.597,24.49,19.231c3.393,2.102,6.263,3.928,9.099,6.078
		c2.803,2.14,5.39,4.531,8.084,7.712c3.256,3.898,6.229,7.791,8.837,11.96c2.191,3.483,4.104,7.18,5.793,11.236
		c3.817-1.539,7.859-3.235,11.095-5.97c2.795-2.397,5.123-5.801,7.444-9.194c2.079-3.055,4.166-6.082,6.645-8.577
		c0.808-0.838,1.73-1.522,2.729-2.025c3.801-2.007,8.612-1.676,13.166-0.104c4.3,1.472,8.45,4.088,11.27,6.87
		c1.297,1.267,2.337,2.624,3.019,3.94c5.664,10.829,9.739,25.885,12.201,41.381C511.441,191.026,512.189,209.058,510.854,222.319z
		 M431.548,151.042c-0.823,0.021-1.63,0.042-2.412,0.071c1.173,1.152,2.092,2.52,2.766,4.032c0.332-0.318,0.711-0.626,1.118-0.904
		C432.537,153.161,432.047,152.09,431.548,151.042z M32.417,199.417c2.094-0.235,3.396,0.865,5.016,2.478
		c-0.293-3.865-1.025-6.734-3.739-8.329c-0.195,0.628-0.349,1.26-0.53,1.898C32.814,196.707,32.598,198.029,32.417,199.417z
		 M40.738,241.592c0.196-0.695,0.401-1.379,0.626-2.078c1.316-4.154,2.726-8.186,4.238-12.095c-3.163,2.859-6.72,5.285-11.035,7.013
		c-11.158,4.464-10.931,0.607-15.734-9.08c10.211-8.155,5.485-18.171,8.735-29.428c0.757-2.587,1.853-5.115,3.404-8.448
		c-10.694,6.438-30.347,24.419-17.214,38.046c1.424,5.142,2.524,9.143,4.479,13.209C22.257,247.129,32.758,244.975,40.738,241.592z
		 M70.303,377.823c-11.416-17.278-21.729-37.847-27.781-58.764c-1.117,2.998-2.237,5.972-3.348,8.966
		c-5.117,5.164-10.232,10.346-15.349,15.531c-3.791,3.847-4.531,3.98-3.764,9.415c1.034,7.461,5.436,15.391,12.012,22.103
		c6.015,6.143,26.779,22.165,34.944,11.245c1.738-2.329,2.556-5.207,3.286-8.176C70.303,378.027,70.303,377.939,70.303,377.823z
		 M321.197,345.116c-1.805-4.291-2.803-8.316-4.059-12.38c-2.716,9.715-6.38,18.859-9.706,29.372
		c-3.152,9.947-19.346,38.791-28.86,45.802c1.854,1.381,5.256,1.913,11.319,2.438c4.292,0.365,21.188,0.465,25.446,0.527
		c9.348,0.171,12.896-0.54,13.844-11.178c0.457-5.219,0.041-6.188-2.563-10.758c-0.224-8.143-0.448-16.302-0.673-24.444
		C325.709,356.781,324.225,352.319,321.197,345.116z M489.177,140.213c-2.175-5.267-10.871-13.16-16.394-7.795
		c-5.572,5.429-9.764,13.07-15.835,17.873c-8.275,6.504-22.506,4.267-19.928,18.09c1.884,10.068,2.42,21.337,0.266,30.717
		c-2.054,9.009-4.191,21.812-8.891,27.843c1.389-5.104,3.726-18.449,4.379-27.452c0.354-4.926-0.199-10.871-1.119-16.854
		c-1.039,0.021-2.074,0.045-3.135,0.077c-3.793,0.077-9.973,5.516-11.636,8.91c-4.641,9.452-4.957,18.235-10.155,27.232
		c4.184-10.057,1.83-19.127,5.651-29.914c1.344-3.805,4.758-7.086,8.704-9.083c-2.196-0.328-4.392,0.632-7.636,1.389
		c-13.881,3.302-13.257,14.212-21.794,24.631c8.229-15.098,4.029-25.109,21.873-29.182c5.872-1.331,10.159-1.865,14.019,1.22
		c0.083,0,0.167,0,0.241-0.017c1.015-0.077,2.021-0.16,3.044-0.231c-0.507-2.771-1.048-5.501-1.588-8.074
		c-8.159-1.069-15.678-0.842-24.045,0.422c1.809-0.786,3.568-1.512,5.273-2.129c2.661-0.977,5.248-1.749,7.821-2.189
		c0.254-0.509,0.403-1.071,0.475-1.676c0.266-2.819-1.821-5.337-4.637-5.616c-2.828-0.268-5.231,1.811-5.506,4.639
		c-0.1,1.11,0.291,2.437,0.915,3.537c-0.138-0.248-0.271-0.493-0.392-0.755l-0.012-0.017c-1.077-2.077-1.568-4.473-1.348-6.957
		c0.195-1.875,0.781-3.604,1.676-5.136c-0.274,0.066-0.549,0.154-0.823,0.251c-1.983,0.796-6.213,0.541-8.126-0.114
		c3.751-0.622,9.34-2.616,12.95-4.375c0.1-0.042,0.195-0.083,0.274-0.133c2.254-1.389,4.956-2.073,7.776-1.811
		c1.431,0.143,2.777,0.513,4.009,1.1c1.68,0.495,3.56,1.163,5.821,1.976c-3.435-7.178-7.314-13.667-11.984-20.177
		c-4.416-6.145-8.126-8.267-14.655-12.124c-1.422-0.844-2.811-1.687-4.183-2.572c-4.824-0.884-10.205-1.753-14.847-0.869
		c3.826-1.505,6.704-2.112,10.056-2.389c-9.24-6.625-17.212-14.06-26.332-21.73c-35.193-29.592-63.671-27.49-108.487-8.032
		c-18.187,7.918-24.697,15.879-35.886,30.204c0.098,0,0.216-0.01,0.35-0.01c11.379-0.287,22.693-0.678,34.391-1.778
		c-12.099,2.817-23.668,5.069-35.547,6.868c-13.108,2-17.133,1.667-27.02,11.289c-18.219,17.724-33.48,37.757-50.156,55.732
		c-9.65,10.417-14.449,20.857-20.115,32.724c-5.62,11.806-4.842,16.278,2.699,26.481c7.704,10.444,12.361,15.168,15.627,24.441
		c8.519-13.149,18.782-24.051,29.603-37.121c-8.835,14.16-16.892,27.54-24.194,41.937c-4.549,8.958-6.783,13.523-6.637,23.43
		c8.539,9.839,13.894,15.087,21.822,17.266c8.54,2.346,16.588,1.955,24.307-2.046c19.204-9.938,37.458-22.905,58.772-24.539
		c11.003-19.611,7.912-44.272,3.963-66.578c-3.271-18.537-2.441-34.884,1.975-53.477c1.106,18.058,2.551,34.826,6.133,52.53
		c5.219,25.935,6.317,48.324-4.59,75.122c-23.556,0.565-42.889,14.367-63.781,25.051c-9.394,4.808-18.181,5.29-28.692,2.79
		c-10.716-2.549-18.932-10.816-29.958-22.889c1.081-9.128,2.272-14.8,5.522-21.503c-3.932-11.844-8.808-17.291-17.821-29.405
		c-10.453-14.043-10.633-20.127-2.726-36.119c5.699-11.594,11.041-22.29,20.055-32.867c8.874-10.479,17.035-20.233,25.284-29.453
		c-34.283,5.204-56.167,13.471-79.582,34.008c-17.36,15.241-28.188,34.165-35.913,56.018c-4.895,13.79-6.267,27.683-3.959,50.226
		c2.967,29.056,18.397,61.838,35.013,85.749c-2.027,14.888-4.655,26.104-8.81,36.745c-4.71,12.068,14.033,15.761,23.088,18.239
		c7.353,2.017,35.279,9.681,38.04-0.216c1.645-5.931,1.645-10.072,1.233-16.327c4.978-9.926,6.49-11.253,10.043-17.124
		c3.801-6.284,4.119-8.792,4.15-16.173c0.015-9.519-0.231-26.519,0.17-32.665c1.289,5.497,2.822,13.149,4.092,20.493
		c19.849,3.551,39.548,3.293,59.384-0.757c0.252-1.385,0.711-2.895,1.225-4.749c1.326-4.642,3.99-9.282,5.331-13.932
		c-0.375,4.579-0.724,9.145-1.096,13.711c-0.37,4.712-0.385,8.513,0.195,13.212c0.32,2.586,0.649,5.165,0.957,7.747
		c-1.435-2.366-3.67-4.637-4.976-7.016c-8.835,5.813-13.694,9.104-22.369,13.682c-9.164,4.849,1.347,22.198,5.131,28.229
		c8.319,13.203,18.254,28.855,33.707,20.68c5.069-2.674,8.036-8.878,11.419-13.345c4.535-2.669,28.612-22.967,32.438-25.18
		c5.398-3.098,24.848-34.965,27.313-42.912c5.439-17.599,11.827-30.993,13.632-49.353c-12.384-5.207-18.405-10.991-27.28-21.425
		c12.476,9.248,23.246,14.192,36.695,18.451c4.902,4.32,9.848,8.688,14.991,13.266c7.456,6.641,14.302,13.411,25.113,13.606
		c11.694,0.2,24.918-8.741,28.32-19.828c1.306-4.262,2.623-8.537,3.925-12.804c-2.312-0.558-4.582-1.181-6.77-1.851
		c-2.425,5.83-2.233,8.089-3.377,12.372c-1.971,7.327-12.056,12.413-22.622,9.863c-3.756-0.906-5.964-1.43-7.303-2.395
		c1.057,2.886,3.127,5.344,6.97,6.046c4.259,0.79,7.402,1.132,13.59-0.274c-7.651,3.968-10.113,4.138-15.536,2.928
		c-14.268-3.169-9.244-20.352-6.134-30.86c1.954-6.616,1.293-13.636-0.188-20.265c5.406,3.418,9.689,6.429,15.84,8.509
		c28.814,9.706,63.006,19.599,92.008,3.397c22.747-12.696,35.219-40.854,38.021-66.647
		C502.908,198.882,498.6,163.013,489.177,140.213z M342.09,278.663c-0.566,3.581-1.335,8.001-1.901,11.582
		c1.522-4.042,3.331-8.721,5.091-12.58c1.867-4.079,2.91-4.449,6.849-6.604c2.812-1.53,7.901-3.626,10.7-5.14
		c-2.882,0.482-8.051,1.534-10.921,2.021C344.21,269.244,343.295,271.086,342.09,278.663z M210.549,140.954
		c-8.926,8.843-17.601,39.161-20.373,51.285c4.356-10.089,15.299-38.256,23.679-45.428c2.314-1.986,3.944-3.204,5.668-4.09
		c-6,10.08-5.564,12.577-3.456,26.062c1.788-13.709,6.545-19.017,14.315-29.232c8.529-2.156,16.53-4.706,25.271-8.12
		c-9.872,1.121-19.713,2.133-29.603,3.065C217.769,135.277,216.432,135.159,210.549,140.954z M358.807,174.577
		c-1.917-4.027-5.285-6.911-9.182-8.332c3.547-1.913,7.011-3.886,9.73-6.35c-7.951,3.683-17.199,2.778-24.003,7.394
		c-6.03,4.042-14.317,16.909-20.418,22.348c4.416-1.713,8.674-4.681,12.592-7.795c0.029,2.314,0.549,4.678,1.589,6.91
		c1.089,2.274,2.669,4.146,4.508,5.626c-0.524-0.524-0.978-1.121-1.314-1.815c-1.63-3.41-0.175-7.512,3.235-9.126
		c3.418-1.646,7.539-0.187,9.169,3.219c0.146,0.329,0.287,0.657,0.379,0.986c-3.269,2.439-6.263,5.288-8.767,8.587
		c-2.187,2.879-4.017,6.129-5.322,9.729c11.619-13.779,28.474-24.203,44.321-28.64c-4.433-0.052-9.681,0.78-15.021,2.518
		C360.112,178.073,359.626,176.289,358.807,174.577z M302.051,162.158c2.674-11.835,7.428-23.242,25.816-32.075
		C303.498,136.186,298.924,146.42,302.051,162.158z">

</path>

</g>

</svg>)svg"},
        {"dbs-hadoop-svgrepo-com", R"svg(<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor"  version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink"  width="800px"
	 height="800px" viewBox="0 0 512 512" enable-background="new 0 0 512 512" xml:space="preserve">

<g id="3e91140ac1bfb9903b91c1b0ca0929ce">

<path display="inline" d="M482.007,132.576c-3.451,13.652-10.421,25.263-24.859,30.405c-7.078,2.52-11.652-0.073-18.871-2.154
		c7.086,0.674,11.161,1.763,17.507-1.378C467.631,153.591,476.36,144.019,482.007,132.576z M510.854,222.319
		c-1.684,16.547-6.101,32.622-13.968,46.247c-7.045,12.218-16.863,22.489-29.941,29.492c-8.887,4.725-19.213,7.461-29.888,8.579
		c-11.082,1.152-22.522,0.536-33.197-1.276c-0.499,1.654-1.015,3.335-1.506,4.978c-0.624,2.066-1.256,4.146-1.88,6.2
		c-2.179,7.161-7.385,13.615-13.872,18.397c-6.695,4.94-14.858,8.167-22.469,8.509c-7.27,0.353-12.888-1.36-17.869-4.301
		c-4.824-2.836-8.949-6.782-13.449-11.128l-5.896-5.672c-1.322-1.289-2.645-2.574-3.983-3.851c-0.142,0.927-0.274,1.813-0.416,2.623
		c2.079,4.205,3.551,8.147,5.048,12.189v0.012c0.944,2.524,1.913,5.119,3.082,7.931c1.742,4.158,3.057,7.627,3.972,11.224
		c0.914,3.631,1.418,7.323,1.53,11.848c0.107,3.855,0.212,7.702,0.319,11.549c0.096,3.63,0.188,7.261,0.304,10.891
		c1.605,2.911,2.554,4.907,3.027,7.111c0.532,2.35,0.479,4.641,0.208,8.375c-0.59,8.393-3.364,13.07-8.025,15.724
		c-4.375,2.479-10.168,2.849-17.333,2.815c-1.09-0.017-2.856-0.017-4.999-0.017c-6.952,0-17.706,0.017-20.996-0.216
		c-4.42-0.32-8.097-0.657-11.182-1.397c-2.379-0.565-4.334-1.39-5.968-2.587c-2.375,1.917-4.462,3.651-6.559,5.377
		c-6.173,5.086-12.439,10.268-14.963,11.947c-0.967,1.489-1.842,2.99-2.645,4.384c-2.591,4.541-4.508,7.9-10.617,11.215
		c-11.998,6.479-21.283,5.914-29.428,0.915c-7.76-4.762-14.17-13.511-20.872-23.849c-3.188-4.907-8.278-12.991-10.756-20.918
		c-2.74-8.774-2.36-17.308,6.381-21.799c0.416-0.216,0.838-0.433,1.233-0.641c0.911-0.474,1.77-0.906,2.628-1.348
		c-3.313,0.175-6.643,0.309-9.956,0.267c-6.504-0.042-13.039-0.49-19.703-1.348c0.108,0.795,0.2,1.431,0.251,1.843
		c0.51,4.42,0.863,7.314,0.443,10.213c-0.422,2.99-1.609,5.651-4.136,9.419c-0.168,0.25-0.734,1.069-1.218,1.755
		c-2.343,3.452-3.728,5.506-6.951,12.638c0.318,3.515,0.422,6.342,0.189,9.157c-0.256,3.032-0.894,6.117-1.973,10.151
		c-2.751,10.172-12.979,12.517-24.311,11.806c-10.398-0.648-21.782-3.921-28.359-5.709c-1.428-0.379-2.586-0.679-3.711-0.974
		c-5.55-1.447-11.53-3.011-16.563-5.298c-10.078-4.6-16.537-11.818-10.247-25.638c1.013-2.191,1.896-4.42,2.699-6.666
		c0.308-0.798,0.51-1.563,0.771-2.361c-5.131,0.798-10.582-0.175-15.89-2.192c-9.286-3.53-18.16-10.275-23.589-15.706
		c-4.341-4.351-7.718-9.032-10.245-13.84c-2.734-5.207-4.483-10.604-5.373-15.919c-1.466-8.699-0.443-9.689,4.699-14.626
		c0.767-0.752,1.626-1.571,1.836-1.775c2.541-2.512,5.065-5.031,7.6-7.531c2.705-2.682,5.441-5.385,8.136-8.08
		c1.239-3.352,2.472-6.695,3.715-10.076c0.799-2.158,1.591-4.333,2.381-6.482c-0.38-2.005-0.724-4.021-1.013-6.084
		c-0.334-2.408-0.597-4.62-0.771-6.616c-0.782-9.194-1.044-17.101-0.771-24.261c0.206-5.461,0.715-10.484,1.553-15.291
		c-4.246,0.946-8.571,0.873-12.49-0.37c-4.843-1.564-9.07-4.907-11.688-10.403c-0.981-2.017-1.753-4.057-2.483-6.36
		c-0.642-2.032-1.235-4.3-1.871-6.87c-3.473-3.578-5.441-7.333-6.094-11.178c-0.776-4.413,0.118-8.814,2.094-13.031
		c1.892-4.053,4.797-7.908,8.159-11.405c9.055-9.405,21.926-16.474,27.916-17.514l4.493-0.786l-1.495,4.304
		c-0.777,2.179-1.686,4.462-2.565,6.709c-0.248,0.653-0.499,1.312-0.771,2.021c2.755,2.179,4.406,5.202,5.177,8.643
		c1.204,5.321,0.426,11.451-0.601,16.038l-1.225,5.491l-3.48-4.402c-0.842-1.038-1.543-2.119-2.217-3.167
		c-0.622-0.957-1.212-1.857-1.815-2.597c-0.479,7.143-1.348,14.306-6.438,19.189c0.144,0.614,0.212,0.942,0.314,1.013
		c0.081,0.042,0.946-0.43,3.034-1.243c3.584-1.368,6.772-3.651,9.771-6.335c3.083-2.776,5.903-5.945,8.756-9.081
		c3.225-6.263,6.916-12.187,11.226-17.821c4.381-5.749,9.395-11.184,15.183-16.312c14.166-12.546,27.947-20.901,43.59-26.954
		c15.312-5.917,32.382-9.646,53.352-13.008c2.639-2.736,5.317-5.446,8.063-8.126c2.946-2.828,5.99-5.656,9.178-8.483
		c4.479-3.938,8.222-6.346,11.945-7.836c3.362-1.368,6.679-1.903,10.407-2.175c6.402-8.535,12.239-15.858,19.106-22.202
		c7.19-6.649,15.447-12.183,26.564-16.857c23.025-9.712,42.546-14.771,61.01-13.611c18.597,1.163,35.996,8.608,54.643,23.993
		c3.519,2.907,7.103,6.047,10.675,9.184c7.814,6.87,15.482,13.597,24.49,19.231c3.393,2.102,6.263,3.928,9.099,6.078
		c2.803,2.14,5.39,4.531,8.084,7.712c3.256,3.898,6.229,7.791,8.837,11.96c2.191,3.483,4.104,7.18,5.793,11.236
		c3.817-1.539,7.859-3.235,11.095-5.97c2.795-2.397,5.123-5.801,7.444-9.194c2.079-3.055,4.166-6.082,6.645-8.577
		c0.808-0.838,1.73-1.522,2.729-2.025c3.801-2.007,8.612-1.676,13.166-0.104c4.3,1.472,8.45,4.088,11.27,6.87
		c1.297,1.267,2.337,2.624,3.019,3.94c5.664,10.829,9.739,25.885,12.201,41.381C511.441,191.026,512.189,209.058,510.854,222.319z
		 M431.548,151.042c-0.823,0.021-1.63,0.042-2.412,0.071c1.173,1.152,2.092,2.52,2.766,4.032c0.332-0.318,0.711-0.626,1.118-0.904
		C432.537,153.161,432.047,152.09,431.548,151.042z M32.417,199.417c2.094-0.235,3.396,0.865,5.016,2.478
		c-0.293-3.865-1.025-6.734-3.739-8.329c-0.195,0.628-0.349,1.26-0.53,1.898C32.814,196.707,32.598,198.029,32.417,199.417z
		 M40.738,241.592c0.196-0.695,0.401-1.379,0.626-2.078c1.316-4.154,2.726-8.186,4.238-12.095c-3.163,2.859-6.72,5.285-11.035,7.013
		c-11.158,4.464-10.931,0.607-15.734-9.08c10.211-8.155,5.485-18.171,8.735-29.428c0.757-2.587,1.853-5.115,3.404-8.448
		c-10.694,6.438-30.347,24.419-17.214,38.046c1.424,5.142,2.524,9.143,4.479,13.209C22.257,247.129,32.758,244.975,40.738,241.592z
		 M70.303,377.823c-11.416-17.278-21.729-37.847-27.781-58.764c-1.117,2.998-2.237,5.972-3.348,8.966
		c-5.117,5.164-10.232,10.346-15.349,15.531c-3.791,3.847-4.531,3.98-3.764,9.415c1.034,7.461,5.436,15.391,12.012,22.103
		c6.015,6.143,26.779,22.165,34.944,11.245c1.738-2.329,2.556-5.207,3.286-8.176C70.303,378.027,70.303,377.939,70.303,377.823z
		 M321.197,345.116c-1.805-4.291-2.803-8.316-4.059-12.38c-2.716,9.715-6.38,18.859-9.706,29.372
		c-3.152,9.947-19.346,38.791-28.86,45.802c1.854,1.381,5.256,1.913,11.319,2.438c4.292,0.365,21.188,0.465,25.446,0.527
		c9.348,0.171,12.896-0.54,13.844-11.178c0.457-5.219,0.041-6.188-2.563-10.758c-0.224-8.143-0.448-16.302-0.673-24.444
		C325.709,356.781,324.225,352.319,321.197,345.116z M489.177,140.213c-2.175-5.267-10.871-13.16-16.394-7.795
		c-5.572,5.429-9.764,13.07-15.835,17.873c-8.275,6.504-22.506,4.267-19.928,18.09c1.884,10.068,2.42,21.337,0.266,30.717
		c-2.054,9.009-4.191,21.812-8.891,27.843c1.389-5.104,3.726-18.449,4.379-27.452c0.354-4.926-0.199-10.871-1.119-16.854
		c-1.039,0.021-2.074,0.045-3.135,0.077c-3.793,0.077-9.973,5.516-11.636,8.91c-4.641,9.452-4.957,18.235-10.155,27.232
		c4.184-10.057,1.83-19.127,5.651-29.914c1.344-3.805,4.758-7.086,8.704-9.083c-2.196-0.328-4.392,0.632-7.636,1.389
		c-13.881,3.302-13.257,14.212-21.794,24.631c8.229-15.098,4.029-25.109,21.873-29.182c5.872-1.331,10.159-1.865,14.019,1.22
		c0.083,0,0.167,0,0.241-0.017c1.015-0.077,2.021-0.16,3.044-0.231c-0.507-2.771-1.048-5.501-1.588-8.074
		c-8.159-1.069-15.678-0.842-24.045,0.422c1.809-0.786,3.568-1.512,5.273-2.129c2.661-0.977,5.248-1.749,7.821-2.189
		c0.254-0.509,0.403-1.071,0.475-1.676c0.266-2.819-1.821-5.337-4.637-5.616c-2.828-0.268-5.231,1.811-5.506,4.639
		c-0.1,1.11,0.291,2.437,0.915,3.537c-0.138-0.248-0.271-0.493-0.392-0.755l-0.012-0.017c-1.077-2.077-1.568-4.473-1.348-6.957
		c0.195-1.875,0.781-3.604,1.676-5.136c-0.274,0.066-0.549,0.154-0.823,0.251c-1.983,0.796-6.213,0.541-8.126-0.114
		c3.751-0.622,9.34-2.616,12.95-4.375c0.1-0.042,0.195-0.083,0.274-0.133c2.254-1.389,4.956-2.073,7.776-1.811
		c1.431,0.143,2.777,0.513,4.009,1.1c1.68,0.495,3.56,1.163,5.821,1.976c-3.435-7.178-7.314-13.667-11.984-20.177
		c-4.416-6.145-8.126-8.267-14.655-12.124c-1.422-0.844-2.811-1.687-4.183-2.572c-4.824-0.884-10.205-1.753-14.847-0.869
		c3.826-1.505,6.704-2.112,10.056-2.389c-9.24-6.625-17.212-14.06-26.332-21.73c-35.193-29.592-63.671-27.49-108.487-8.032
		c-18.187,7.918-24.697,15.879-35.886,30.204c0.098,0,0.216-0.01,0.35-0.01c11.379-0.287,22.693-0.678,34.391-1.778
		c-12.099,2.817-23.668,5.069-35.547,6.868c-13.108,2-17.133,1.667-27.02,11.289c-18.219,17.724-33.48,37.757-50.156,55.732
		c-9.65,10.417-14.449,20.857-20.115,32.724c-5.62,11.806-4.842,16.278,2.699,26.481c7.704,10.444,12.361,15.168,15.627,24.441
		c8.519-13.149,18.782-24.051,29.603-37.121c-8.835,14.16-16.892,27.54-24.194,41.937c-4.549,8.958-6.783,13.523-6.637,23.43
		c8.539,9.839,13.894,15.087,21.822,17.266c8.54,2.346,16.588,1.955,24.307-2.046c19.204-9.938,37.458-22.905,58.772-24.539
		c11.003-19.611,7.912-44.272,3.963-66.578c-3.271-18.537-2.441-34.884,1.975-53.477c1.106,18.058,2.551,34.826,6.133,52.53
		c5.219,25.935,6.317,48.324-4.59,75.122c-23.556,0.565-42.889,14.367-63.781,25.051c-9.394,4.808-18.181,5.29-28.692,2.79
		c-10.716-2.549-18.932-10.816-29.958-22.889c1.081-9.128,2.272-14.8,5.522-21.503c-3.932-11.844-8.808-17.291-17.821-29.405
		c-10.453-14.043-10.633-20.127-2.726-36.119c5.699-11.594,11.041-22.29,20.055-32.867c8.874-10.479,17.035-20.233,25.284-29.453
		c-34.283,5.204-56.167,13.471-79.582,34.008c-17.36,15.241-28.188,34.165-35.913,56.018c-4.895,13.79-6.267,27.683-3.959,50.226
		c2.967,29.056,18.397,61.838,35.013,85.749c-2.027,14.888-4.655,26.104-8.81,36.745c-4.71,12.068,14.033,15.761,23.088,18.239
		c7.353,2.017,35.279,9.681,38.04-0.216c1.645-5.931,1.645-10.072,1.233-16.327c4.978-9.926,6.49-11.253,10.043-17.124
		c3.801-6.284,4.119-8.792,4.15-16.173c0.015-9.519-0.231-26.519,0.17-32.665c1.289,5.497,2.822,13.149,4.092,20.493
		c19.849,3.551,39.548,3.293,59.384-0.757c0.252-1.385,0.711-2.895,1.225-4.749c1.326-4.642,3.99-9.282,5.331-13.932
		c-0.375,4.579-0.724,9.145-1.096,13.711c-0.37,4.712-0.385,8.513,0.195,13.212c0.32,2.586,0.649,5.165,0.957,7.747
		c-1.435-2.366-3.67-4.637-4.976-7.016c-8.835,5.813-13.694,9.104-22.369,13.682c-9.164,4.849,1.347,22.198,5.131,28.229
		c8.319,13.203,18.254,28.855,33.707,20.68c5.069-2.674,8.036-8.878,11.419-13.345c4.535-2.669,28.612-22.967,32.438-25.18
		c5.398-3.098,24.848-34.965,27.313-42.912c5.439-17.599,11.827-30.993,13.632-49.353c-12.384-5.207-18.405-10.991-27.28-21.425
		c12.476,9.248,23.246,14.192,36.695,18.451c4.902,4.32,9.848,8.688,14.991,13.266c7.456,6.641,14.302,13.411,25.113,13.606
		c11.694,0.2,24.918-8.741,28.32-19.828c1.306-4.262,2.623-8.537,3.925-12.804c-2.312-0.558-4.582-1.181-6.77-1.851
		c-2.425,5.83-2.233,8.089-3.377,12.372c-1.971,7.327-12.056,12.413-22.622,9.863c-3.756-0.906-5.964-1.43-7.303-2.395
		c1.057,2.886,3.127,5.344,6.97,6.046c4.259,0.79,7.402,1.132,13.59-0.274c-7.651,3.968-10.113,4.138-15.536,2.928
		c-14.268-3.169-9.244-20.352-6.134-30.86c1.954-6.616,1.293-13.636-0.188-20.265c5.406,3.418,9.689,6.429,15.84,8.509
		c28.814,9.706,63.006,19.599,92.008,3.397c22.747-12.696,35.219-40.854,38.021-66.647
		C502.908,198.882,498.6,163.013,489.177,140.213z M342.09,278.663c-0.566,3.581-1.335,8.001-1.901,11.582
		c1.522-4.042,3.331-8.721,5.091-12.58c1.867-4.079,2.91-4.449,6.849-6.604c2.812-1.53,7.901-3.626,10.7-5.14
		c-2.882,0.482-8.051,1.534-10.921,2.021C344.21,269.244,343.295,271.086,342.09,278.663z M210.549,140.954
		c-8.926,8.843-17.601,39.161-20.373,51.285c4.356-10.089,15.299-38.256,23.679-45.428c2.314-1.986,3.944-3.204,5.668-4.09
		c-6,10.08-5.564,12.577-3.456,26.062c1.788-13.709,6.545-19.017,14.315-29.232c8.529-2.156,16.53-4.706,25.271-8.12
		c-9.872,1.121-19.713,2.133-29.603,3.065C217.769,135.277,216.432,135.159,210.549,140.954z M358.807,174.577
		c-1.917-4.027-5.285-6.911-9.182-8.332c3.547-1.913,7.011-3.886,9.73-6.35c-7.951,3.683-17.199,2.778-24.003,7.394
		c-6.03,4.042-14.317,16.909-20.418,22.348c4.416-1.713,8.674-4.681,12.592-7.795c0.029,2.314,0.549,4.678,1.589,6.91
		c1.089,2.274,2.669,4.146,4.508,5.626c-0.524-0.524-0.978-1.121-1.314-1.815c-1.63-3.41-0.175-7.512,3.235-9.126
		c3.418-1.646,7.539-0.187,9.169,3.219c0.146,0.329,0.287,0.657,0.379,0.986c-3.269,2.439-6.263,5.288-8.767,8.587
		c-2.187,2.879-4.017,6.129-5.322,9.729c11.619-13.779,28.474-24.203,44.321-28.64c-4.433-0.052-9.681,0.78-15.021,2.518
		C360.112,178.073,359.626,176.289,358.807,174.577z M302.051,162.158c2.674-11.835,7.428-23.242,25.816-32.075
		C303.498,136.186,298.924,146.42,302.051,162.158z">

</path>

</g>

</svg>)svg"},
        {"dbs_hadoop", R"svg(<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor"  version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink"  width="800px"
	 height="800px" viewBox="0 0 512 512" enable-background="new 0 0 512 512" xml:space="preserve">

<g id="3e91140ac1bfb9903b91c1b0ca0929ce">

<path display="inline" d="M482.007,132.576c-3.451,13.652-10.421,25.263-24.859,30.405c-7.078,2.52-11.652-0.073-18.871-2.154
		c7.086,0.674,11.161,1.763,17.507-1.378C467.631,153.591,476.36,144.019,482.007,132.576z M510.854,222.319
		c-1.684,16.547-6.101,32.622-13.968,46.247c-7.045,12.218-16.863,22.489-29.941,29.492c-8.887,4.725-19.213,7.461-29.888,8.579
		c-11.082,1.152-22.522,0.536-33.197-1.276c-0.499,1.654-1.015,3.335-1.506,4.978c-0.624,2.066-1.256,4.146-1.88,6.2
		c-2.179,7.161-7.385,13.615-13.872,18.397c-6.695,4.94-14.858,8.167-22.469,8.509c-7.27,0.353-12.888-1.36-17.869-4.301
		c-4.824-2.836-8.949-6.782-13.449-11.128l-5.896-5.672c-1.322-1.289-2.645-2.574-3.983-3.851c-0.142,0.927-0.274,1.813-0.416,2.623
		c2.079,4.205,3.551,8.147,5.048,12.189v0.012c0.944,2.524,1.913,5.119,3.082,7.931c1.742,4.158,3.057,7.627,3.972,11.224
		c0.914,3.631,1.418,7.323,1.53,11.848c0.107,3.855,0.212,7.702,0.319,11.549c0.096,3.63,0.188,7.261,0.304,10.891
		c1.605,2.911,2.554,4.907,3.027,7.111c0.532,2.35,0.479,4.641,0.208,8.375c-0.59,8.393-3.364,13.07-8.025,15.724
		c-4.375,2.479-10.168,2.849-17.333,2.815c-1.09-0.017-2.856-0.017-4.999-0.017c-6.952,0-17.706,0.017-20.996-0.216
		c-4.42-0.32-8.097-0.657-11.182-1.397c-2.379-0.565-4.334-1.39-5.968-2.587c-2.375,1.917-4.462,3.651-6.559,5.377
		c-6.173,5.086-12.439,10.268-14.963,11.947c-0.967,1.489-1.842,2.99-2.645,4.384c-2.591,4.541-4.508,7.9-10.617,11.215
		c-11.998,6.479-21.283,5.914-29.428,0.915c-7.76-4.762-14.17-13.511-20.872-23.849c-3.188-4.907-8.278-12.991-10.756-20.918
		c-2.74-8.774-2.36-17.308,6.381-21.799c0.416-0.216,0.838-0.433,1.233-0.641c0.911-0.474,1.77-0.906,2.628-1.348
		c-3.313,0.175-6.643,0.309-9.956,0.267c-6.504-0.042-13.039-0.49-19.703-1.348c0.108,0.795,0.2,1.431,0.251,1.843
		c0.51,4.42,0.863,7.314,0.443,10.213c-0.422,2.99-1.609,5.651-4.136,9.419c-0.168,0.25-0.734,1.069-1.218,1.755
		c-2.343,3.452-3.728,5.506-6.951,12.638c0.318,3.515,0.422,6.342,0.189,9.157c-0.256,3.032-0.894,6.117-1.973,10.151
		c-2.751,10.172-12.979,12.517-24.311,11.806c-10.398-0.648-21.782-3.921-28.359-5.709c-1.428-0.379-2.586-0.679-3.711-0.974
		c-5.55-1.447-11.53-3.011-16.563-5.298c-10.078-4.6-16.537-11.818-10.247-25.638c1.013-2.191,1.896-4.42,2.699-6.666
		c0.308-0.798,0.51-1.563,0.771-2.361c-5.131,0.798-10.582-0.175-15.89-2.192c-9.286-3.53-18.16-10.275-23.589-15.706
		c-4.341-4.351-7.718-9.032-10.245-13.84c-2.734-5.207-4.483-10.604-5.373-15.919c-1.466-8.699-0.443-9.689,4.699-14.626
		c0.767-0.752,1.626-1.571,1.836-1.775c2.541-2.512,5.065-5.031,7.6-7.531c2.705-2.682,5.441-5.385,8.136-8.08
		c1.239-3.352,2.472-6.695,3.715-10.076c0.799-2.158,1.591-4.333,2.381-6.482c-0.38-2.005-0.724-4.021-1.013-6.084
		c-0.334-2.408-0.597-4.62-0.771-6.616c-0.782-9.194-1.044-17.101-0.771-24.261c0.206-5.461,0.715-10.484,1.553-15.291
		c-4.246,0.946-8.571,0.873-12.49-0.37c-4.843-1.564-9.07-4.907-11.688-10.403c-0.981-2.017-1.753-4.057-2.483-6.36
		c-0.642-2.032-1.235-4.3-1.871-6.87c-3.473-3.578-5.441-7.333-6.094-11.178c-0.776-4.413,0.118-8.814,2.094-13.031
		c1.892-4.053,4.797-7.908,8.159-11.405c9.055-9.405,21.926-16.474,27.916-17.514l4.493-0.786l-1.495,4.304
		c-0.777,2.179-1.686,4.462-2.565,6.709c-0.248,0.653-0.499,1.312-0.771,2.021c2.755,2.179,4.406,5.202,5.177,8.643
		c1.204,5.321,0.426,11.451-0.601,16.038l-1.225,5.491l-3.48-4.402c-0.842-1.038-1.543-2.119-2.217-3.167
		c-0.622-0.957-1.212-1.857-1.815-2.597c-0.479,7.143-1.348,14.306-6.438,19.189c0.144,0.614,0.212,0.942,0.314,1.013
		c0.081,0.042,0.946-0.43,3.034-1.243c3.584-1.368,6.772-3.651,9.771-6.335c3.083-2.776,5.903-5.945,8.756-9.081
		c3.225-6.263,6.916-12.187,11.226-17.821c4.381-5.749,9.395-11.184,15.183-16.312c14.166-12.546,27.947-20.901,43.59-26.954
		c15.312-5.917,32.382-9.646,53.352-13.008c2.639-2.736,5.317-5.446,8.063-8.126c2.946-2.828,5.99-5.656,9.178-8.483
		c4.479-3.938,8.222-6.346,11.945-7.836c3.362-1.368,6.679-1.903,10.407-2.175c6.402-8.535,12.239-15.858,19.106-22.202
		c7.19-6.649,15.447-12.183,26.564-16.857c23.025-9.712,42.546-14.771,61.01-13.611c18.597,1.163,35.996,8.608,54.643,23.993
		c3.519,2.907,7.103,6.047,10.675,9.184c7.814,6.87,15.482,13.597,24.49,19.231c3.393,2.102,6.263,3.928,9.099,6.078
		c2.803,2.14,5.39,4.531,8.084,7.712c3.256,3.898,6.229,7.791,8.837,11.96c2.191,3.483,4.104,7.18,5.793,11.236
		c3.817-1.539,7.859-3.235,11.095-5.97c2.795-2.397,5.123-5.801,7.444-9.194c2.079-3.055,4.166-6.082,6.645-8.577
		c0.808-0.838,1.73-1.522,2.729-2.025c3.801-2.007,8.612-1.676,13.166-0.104c4.3,1.472,8.45,4.088,11.27,6.87
		c1.297,1.267,2.337,2.624,3.019,3.94c5.664,10.829,9.739,25.885,12.201,41.381C511.441,191.026,512.189,209.058,510.854,222.319z
		 M431.548,151.042c-0.823,0.021-1.63,0.042-2.412,0.071c1.173,1.152,2.092,2.52,2.766,4.032c0.332-0.318,0.711-0.626,1.118-0.904
		C432.537,153.161,432.047,152.09,431.548,151.042z M32.417,199.417c2.094-0.235,3.396,0.865,5.016,2.478
		c-0.293-3.865-1.025-6.734-3.739-8.329c-0.195,0.628-0.349,1.26-0.53,1.898C32.814,196.707,32.598,198.029,32.417,199.417z
		 M40.738,241.592c0.196-0.695,0.401-1.379,0.626-2.078c1.316-4.154,2.726-8.186,4.238-12.095c-3.163,2.859-6.72,5.285-11.035,7.013
		c-11.158,4.464-10.931,0.607-15.734-9.08c10.211-8.155,5.485-18.171,8.735-29.428c0.757-2.587,1.853-5.115,3.404-8.448
		c-10.694,6.438-30.347,24.419-17.214,38.046c1.424,5.142,2.524,9.143,4.479,13.209C22.257,247.129,32.758,244.975,40.738,241.592z
		 M70.303,377.823c-11.416-17.278-21.729-37.847-27.781-58.764c-1.117,2.998-2.237,5.972-3.348,8.966
		c-5.117,5.164-10.232,10.346-15.349,15.531c-3.791,3.847-4.531,3.98-3.764,9.415c1.034,7.461,5.436,15.391,12.012,22.103
		c6.015,6.143,26.779,22.165,34.944,11.245c1.738-2.329,2.556-5.207,3.286-8.176C70.303,378.027,70.303,377.939,70.303,377.823z
		 M321.197,345.116c-1.805-4.291-2.803-8.316-4.059-12.38c-2.716,9.715-6.38,18.859-9.706,29.372
		c-3.152,9.947-19.346,38.791-28.86,45.802c1.854,1.381,5.256,1.913,11.319,2.438c4.292,0.365,21.188,0.465,25.446,0.527
		c9.348,0.171,12.896-0.54,13.844-11.178c0.457-5.219,0.041-6.188-2.563-10.758c-0.224-8.143-0.448-16.302-0.673-24.444
		C325.709,356.781,324.225,352.319,321.197,345.116z M489.177,140.213c-2.175-5.267-10.871-13.16-16.394-7.795
		c-5.572,5.429-9.764,13.07-15.835,17.873c-8.275,6.504-22.506,4.267-19.928,18.09c1.884,10.068,2.42,21.337,0.266,30.717
		c-2.054,9.009-4.191,21.812-8.891,27.843c1.389-5.104,3.726-18.449,4.379-27.452c0.354-4.926-0.199-10.871-1.119-16.854
		c-1.039,0.021-2.074,0.045-3.135,0.077c-3.793,0.077-9.973,5.516-11.636,8.91c-4.641,9.452-4.957,18.235-10.155,27.232
		c4.184-10.057,1.83-19.127,5.651-29.914c1.344-3.805,4.758-7.086,8.704-9.083c-2.196-0.328-4.392,0.632-7.636,1.389
		c-13.881,3.302-13.257,14.212-21.794,24.631c8.229-15.098,4.029-25.109,21.873-29.182c5.872-1.331,10.159-1.865,14.019,1.22
		c0.083,0,0.167,0,0.241-0.017c1.015-0.077,2.021-0.16,3.044-0.231c-0.507-2.771-1.048-5.501-1.588-8.074
		c-8.159-1.069-15.678-0.842-24.045,0.422c1.809-0.786,3.568-1.512,5.273-2.129c2.661-0.977,5.248-1.749,7.821-2.189
		c0.254-0.509,0.403-1.071,0.475-1.676c0.266-2.819-1.821-5.337-4.637-5.616c-2.828-0.268-5.231,1.811-5.506,4.639
		c-0.1,1.11,0.291,2.437,0.915,3.537c-0.138-0.248-0.271-0.493-0.392-0.755l-0.012-0.017c-1.077-2.077-1.568-4.473-1.348-6.957
		c0.195-1.875,0.781-3.604,1.676-5.136c-0.274,0.066-0.549,0.154-0.823,0.251c-1.983,0.796-6.213,0.541-8.126-0.114
		c3.751-0.622,9.34-2.616,12.95-4.375c0.1-0.042,0.195-0.083,0.274-0.133c2.254-1.389,4.956-2.073,7.776-1.811
		c1.431,0.143,2.777,0.513,4.009,1.1c1.68,0.495,3.56,1.163,5.821,1.976c-3.435-7.178-7.314-13.667-11.984-20.177
		c-4.416-6.145-8.126-8.267-14.655-12.124c-1.422-0.844-2.811-1.687-4.183-2.572c-4.824-0.884-10.205-1.753-14.847-0.869
		c3.826-1.505,6.704-2.112,10.056-2.389c-9.24-6.625-17.212-14.06-26.332-21.73c-35.193-29.592-63.671-27.49-108.487-8.032
		c-18.187,7.918-24.697,15.879-35.886,30.204c0.098,0,0.216-0.01,0.35-0.01c11.379-0.287,22.693-0.678,34.391-1.778
		c-12.099,2.817-23.668,5.069-35.547,6.868c-13.108,2-17.133,1.667-27.02,11.289c-18.219,17.724-33.48,37.757-50.156,55.732
		c-9.65,10.417-14.449,20.857-20.115,32.724c-5.62,11.806-4.842,16.278,2.699,26.481c7.704,10.444,12.361,15.168,15.627,24.441
		c8.519-13.149,18.782-24.051,29.603-37.121c-8.835,14.16-16.892,27.54-24.194,41.937c-4.549,8.958-6.783,13.523-6.637,23.43
		c8.539,9.839,13.894,15.087,21.822,17.266c8.54,2.346,16.588,1.955,24.307-2.046c19.204-9.938,37.458-22.905,58.772-24.539
		c11.003-19.611,7.912-44.272,3.963-66.578c-3.271-18.537-2.441-34.884,1.975-53.477c1.106,18.058,2.551,34.826,6.133,52.53
		c5.219,25.935,6.317,48.324-4.59,75.122c-23.556,0.565-42.889,14.367-63.781,25.051c-9.394,4.808-18.181,5.29-28.692,2.79
		c-10.716-2.549-18.932-10.816-29.958-22.889c1.081-9.128,2.272-14.8,5.522-21.503c-3.932-11.844-8.808-17.291-17.821-29.405
		c-10.453-14.043-10.633-20.127-2.726-36.119c5.699-11.594,11.041-22.29,20.055-32.867c8.874-10.479,17.035-20.233,25.284-29.453
		c-34.283,5.204-56.167,13.471-79.582,34.008c-17.36,15.241-28.188,34.165-35.913,56.018c-4.895,13.79-6.267,27.683-3.959,50.226
		c2.967,29.056,18.397,61.838,35.013,85.749c-2.027,14.888-4.655,26.104-8.81,36.745c-4.71,12.068,14.033,15.761,23.088,18.239
		c7.353,2.017,35.279,9.681,38.04-0.216c1.645-5.931,1.645-10.072,1.233-16.327c4.978-9.926,6.49-11.253,10.043-17.124
		c3.801-6.284,4.119-8.792,4.15-16.173c0.015-9.519-0.231-26.519,0.17-32.665c1.289,5.497,2.822,13.149,4.092,20.493
		c19.849,3.551,39.548,3.293,59.384-0.757c0.252-1.385,0.711-2.895,1.225-4.749c1.326-4.642,3.99-9.282,5.331-13.932
		c-0.375,4.579-0.724,9.145-1.096,13.711c-0.37,4.712-0.385,8.513,0.195,13.212c0.32,2.586,0.649,5.165,0.957,7.747
		c-1.435-2.366-3.67-4.637-4.976-7.016c-8.835,5.813-13.694,9.104-22.369,13.682c-9.164,4.849,1.347,22.198,5.131,28.229
		c8.319,13.203,18.254,28.855,33.707,20.68c5.069-2.674,8.036-8.878,11.419-13.345c4.535-2.669,28.612-22.967,32.438-25.18
		c5.398-3.098,24.848-34.965,27.313-42.912c5.439-17.599,11.827-30.993,13.632-49.353c-12.384-5.207-18.405-10.991-27.28-21.425
		c12.476,9.248,23.246,14.192,36.695,18.451c4.902,4.32,9.848,8.688,14.991,13.266c7.456,6.641,14.302,13.411,25.113,13.606
		c11.694,0.2,24.918-8.741,28.32-19.828c1.306-4.262,2.623-8.537,3.925-12.804c-2.312-0.558-4.582-1.181-6.77-1.851
		c-2.425,5.83-2.233,8.089-3.377,12.372c-1.971,7.327-12.056,12.413-22.622,9.863c-3.756-0.906-5.964-1.43-7.303-2.395
		c1.057,2.886,3.127,5.344,6.97,6.046c4.259,0.79,7.402,1.132,13.59-0.274c-7.651,3.968-10.113,4.138-15.536,2.928
		c-14.268-3.169-9.244-20.352-6.134-30.86c1.954-6.616,1.293-13.636-0.188-20.265c5.406,3.418,9.689,6.429,15.84,8.509
		c28.814,9.706,63.006,19.599,92.008,3.397c22.747-12.696,35.219-40.854,38.021-66.647
		C502.908,198.882,498.6,163.013,489.177,140.213z M342.09,278.663c-0.566,3.581-1.335,8.001-1.901,11.582
		c1.522-4.042,3.331-8.721,5.091-12.58c1.867-4.079,2.91-4.449,6.849-6.604c2.812-1.53,7.901-3.626,10.7-5.14
		c-2.882,0.482-8.051,1.534-10.921,2.021C344.21,269.244,343.295,271.086,342.09,278.663z M210.549,140.954
		c-8.926,8.843-17.601,39.161-20.373,51.285c4.356-10.089,15.299-38.256,23.679-45.428c2.314-1.986,3.944-3.204,5.668-4.09
		c-6,10.08-5.564,12.577-3.456,26.062c1.788-13.709,6.545-19.017,14.315-29.232c8.529-2.156,16.53-4.706,25.271-8.12
		c-9.872,1.121-19.713,2.133-29.603,3.065C217.769,135.277,216.432,135.159,210.549,140.954z M358.807,174.577
		c-1.917-4.027-5.285-6.911-9.182-8.332c3.547-1.913,7.011-3.886,9.73-6.35c-7.951,3.683-17.199,2.778-24.003,7.394
		c-6.03,4.042-14.317,16.909-20.418,22.348c4.416-1.713,8.674-4.681,12.592-7.795c0.029,2.314,0.549,4.678,1.589,6.91
		c1.089,2.274,2.669,4.146,4.508,5.626c-0.524-0.524-0.978-1.121-1.314-1.815c-1.63-3.41-0.175-7.512,3.235-9.126
		c3.418-1.646,7.539-0.187,9.169,3.219c0.146,0.329,0.287,0.657,0.379,0.986c-3.269,2.439-6.263,5.288-8.767,8.587
		c-2.187,2.879-4.017,6.129-5.322,9.729c11.619-13.779,28.474-24.203,44.321-28.64c-4.433-0.052-9.681,0.78-15.021,2.518
		C360.112,178.073,359.626,176.289,358.807,174.577z M302.051,162.158c2.674-11.835,7.428-23.242,25.816-32.075
		C303.498,136.186,298.924,146.42,302.051,162.158z">

</path>

</g>

</svg>)svg"},
        {"dbs-postgresql-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor"  version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink"  width="800px"
	 height="800px" viewBox="0 0 512 512" enable-background="new 0 0 512 512" xml:space="preserve">

<g id="3e91140ac1bfb9903b91c1b0ca09020a">

<path display="inline" d="M400.264,168.995c1.028-6.661,1.993-13.04,2.114-20.066c-16.78-2.065-36.036-1.596-43.639,9.272
		c-14.791,21.138,14.097,72.94,26.432,95.061c3.17,5.683,5.461,9.789,6.514,12.347c1.166,2.831,2.44,5.385,3.755,7.728
		c8.487-18.056,6.297-36.352,4.163-54.124c-1.057-8.799-2.154-17.898-1.867-26.649C398.021,183.57,399.163,176.16,400.264,168.995z
		 M386.131,164.883c-1.363,1.436-4.243,3.927-8.172,4.472c-0.505,0.07-1.021,0.106-1.533,0.106c-5.792,0-10.79-4.52-11.205-7.45
		c-0.476-3.535,5.292-6.224,11.245-7.053c1.303-0.181,2.598-0.273,3.84-0.273c5.144,0,8.552,1.526,8.895,3.98
		C389.422,160.252,388.22,162.692,386.131,164.883z M327.039,33.711c10.601-2.196,25.097-4.245,41.328-3.889
		c41.125,0.91,73.651,16.294,96.683,45.725c17.667,22.573-1.782,125.291-58.099,213.903c-0.553-0.706-1.121-1.42-1.706-2.15
		c-0.233-0.294-0.472-0.593-0.714-0.895c14.553-24.032,11.705-47.806,9.173-68.887c-1.041-8.652-2.021-16.824-1.771-24.498
		c0.259-8.14,1.335-15.115,2.376-21.863c1.278-8.313,2.581-16.915,2.219-27.055c0.27-1.063,0.379-2.319,0.237-3.812
		c-0.915-9.728-12.02-38.829-34.651-65.174C369.738,60.709,351.685,44.587,327.039,33.711z M171.328,292.222
		c1.615,1.686,3.301,3.267,5.044,4.759c-6.948,7.438-22.048,23.891-38.119,43.224c-11.372,13.674-19.226,11.06-21.809,10.196
		c-16.828-5.614-36.354-41.182-53.569-97.587c-14.896-48.802-23.603-97.875-24.29-111.638c-2.176-43.52,8.373-73.852,31.356-90.151
		c37.4-26.526,98.897-10.648,123.607-2.595c-0.357,0.349-0.725,0.677-1.075,1.032c-40.549,40.952-39.587,110.918-39.486,115.193
		c-0.006,1.649,0.133,3.984,0.323,7.199c0.697,11.766,1.996,33.668-1.473,58.473C148.615,253.376,155.718,275.934,171.328,292.222z
		 M397.933,301.079c-8.035,2.319-22.459,7.667-21.144,34.405c-1.057,13.407-8.6,76.218-12.428,98.418
		c-5.054,29.324-15.848,40.246-46.18,46.749c-31.457,6.743-42.586-9.31-49.927-27.654c-4.735-11.847-7.059-65.254-5.413-124.232
		c0.024-0.782-0.089-1.541-0.307-2.251c-0.186-1.375-0.476-2.767-0.879-4.17c-2.452-8.584-8.438-15.759-15.617-18.736
		c-2.247-0.932-5.644-2.335-9.822-2.335c-1.543,0-3.062,0.193-4.558,0.581c1.343-5.526,3.669-11.762,6.189-18.519l1.059-2.843
		c1.19-3.207,2.687-6.526,4.268-10.044c8.545-18.981,20.248-44.984,7.547-103.724c-4.758-22.001-20.647-32.744-44.728-30.25
		c-14.44,1.495-27.647,7.321-34.236,10.661c-1.422,0.72-2.727,1.417-3.941,2.095c1.817-22.136,8.735-63.497,34.751-89.773
		c16.366-16.531,38.163-24.695,64.748-24.256c52.314,0.859,85.861,27.706,104.795,50.078c16.315,19.28,25.148,38.699,28.674,49.173
		c-26.504-2.696-44.546,2.539-53.686,15.607c-19.89,28.429,10.878,83.603,25.665,110.121c2.71,4.862,5.054,9.058,5.788,10.845
		c4.812,11.669,11.048,19.461,15.605,25.148C395.549,297.917,396.9,299.61,397.933,301.079z M248.003,326.252
		c0.581,2.037,1.238,5.901-0.907,8.902c-10.808,15.134-25.686,22.81-44.22,22.81c-6.095,0-12.566-0.848-19.24-2.518
		c-3.904-0.976-7.415-2.67-9.477-4.073c1.721-0.812,4.784-1.912,10.096-3.005c25.71-5.297,29.679-9.031,38.349-20.039
		c1.989-2.524,4.244-5.385,7.367-8.874c2.358-2.638,4.46-3.977,6.246-3.977c1.253,0,2.666,0.54,4.383,1.25
		C243.724,318.027,246.769,321.94,248.003,326.252z M469.358,317.431c0.108-0.024,0.218-0.049,0.322-0.068
		c-3.937,3.683-10.669,8.619-20.224,13.057c-8.213,3.812-21.951,6.671-35.007,7.284c-14.42,0.67-21.761-1.613-23.487-3.025
		c-0.807-16.658,5.396-18.4,11.959-20.244c1.028-0.29,2.037-0.572,3.005-0.911c0.605,0.488,1.267,0.979,1.993,1.456
		C419.508,322.63,440.18,323.453,469.358,317.431z M224.536,278.37l1.055-2.823c1.357-3.658,2.948-7.188,4.633-10.931
		c8.163-18.14,18.326-40.719,6.614-94.891c-2.265-10.464-7.708-16.544-16.642-18.591c-19.575-4.485-47.027,10.223-52.95,15.475
		c0.055,1.186,0.144,2.69,0.244,4.404c0.728,12.258,2.081,35.077-1.583,61.281c-2.616,18.712,3.098,36.972,15.676,50.093
		c9.895,10.325,22.902,16.037,36.275,16.077C219.396,292.156,221.874,285.518,224.536,278.37z M206.106,168.942
		c-0.917-1.216-2.411-3.575-2.109-5.707c0.428-3.124,4.188-4.998,10.058-4.998c1.301,0,2.648,0.097,4.008,0.287
		c6.336,0.881,12.875,3.882,12.245,8.386c-0.474,3.412-5.877,8.472-12.366,8.472c-0.567,0-1.14-0.04-1.698-0.119
		C212.492,174.745,208.7,172.381,206.106,168.942z M503.183,308.227c-2.113-9.584-10.793-20.777-28.299-20.777
		c-3.513,0-7.32,0.432-11.641,1.327c-8.301,1.71-15.706,2.775-22.071,3.171c22.293-38.223,40.23-81.256,50.633-121.528
		c9.628-37.277,16.912-87.071-3.054-112.586C459.399,20.326,416.475,0.501,364.616,0.501c-29.228,0-53.098,6.53-60.326,8.74
		c-11.398-1.988-23.668-3.102-36.482-3.31c-0.855-0.014-1.702-0.02-2.546-0.02c-23.059,0-43.723,5.237-61.434,15.567
		c-16.13-5.401-47.269-14.147-79.702-14.147c-36.481,0-65.214,10.757-85.396,31.976C16.385,62.799,6.212,97.864,8.497,143.529
		c0.956,19.149,11.989,76.369,27.785,126.056c24.404,76.774,51.286,112.527,84.595,112.527c10.429,0,20.326-3.65,29.448-10.854
		c7.999,8.717,19.609,12.653,25.994,14.251c9.058,2.267,17.989,3.416,26.547,3.416c11.437,0,22.208-1.997,32.061-5.941
		c0.064,2.142,0.123,4.171,0.173,6.002c0.117,4.162,0.23,8.244,0.384,12.061c1.04,25.951,2.815,46.143,8.089,60.381
		c0.278,0.747,0.649,1.888,1.053,3.122c2.305,7.067,5.788,17.739,13.801,27.428c10.563,12.771,25.616,19.522,43.525,19.522
		c5.72,0,11.847-0.689,18.215-2.057c19.248-4.122,41.114-10.423,57.019-33.067c14.911-21.232,22.144-53,23.454-103.008l0.577-4.921
		l2.775,0.227c1.758,0.08,3.577,0.116,5.408,0.116c18.672,0,39.363-4.005,52.723-10.208
		C473.222,353.426,508.999,334.565,503.183,308.227z M455.442,343.302c-11.487,5.332-29.791,8.704-45.659,8.704
		c-7.611,0-14.649-0.782-20.119-2.484c-2.59,23.806-8.096,68.198-11.302,86.801c-6.002,34.785-21.503,50.564-57.204,58.22
		c-6.078,1.299-11.604,1.88-16.622,1.884c-34.212,0.004-45.038-27.077-49.469-38.153c-5.413-13.536-7.294-57.953-6.913-103.031
		c-12.282,11.116-27.68,16.929-45.278,16.929c-7.256,0-14.888-0.988-22.685-2.94c-2.255-0.565-22.035-5.873-21.771-18.519
		c0.239-11.572,16.753-14.977,22.969-16.259c21.854-4.502,23.266-6.292,30.056-14.912c1.373-1.746,2.986-3.787,4.953-6.119
		c-0.022-0.258-0.051-0.512-0.066-0.77c-9.8-0.263-19.379-2.876-28.1-7.556c-5.899,6.28-21.91,23.564-39.055,44.191
		c-10.323,12.411-20.244,16.058-28.337,16.058c-3.3,0.004-6.295-0.604-8.893-1.473c-22.182-7.396-43.266-43.368-62.66-106.91
		c-14.736-48.277-24.072-98.686-24.891-115.076c-2.437-48.707,10.132-83.181,37.356-102.47
		c48.744-34.534,127.507-8.015,144.689-1.577c16.941-11.55,37.393-17.215,61.105-16.842c12.92,0.212,24.782,1.874,35.665,4.58
		c5.171-2.269,31.844-10.338,63.378-9.991c45.897,0.488,83.813,18.193,109.647,51.204c17.433,22.279,5.93,76.516,0.169,98.799
		c-11.971,46.351-34.163,96.026-61.14,137.128c4.082,3.001,16.518,7.962,51.218,0.798c11.688-2.412,18.724-0.282,20.909,6.325
		C491.615,322.609,468.342,337.315,455.442,343.302z">

</path>

</g>

</svg>)svg"},
        {"dbs-postgresql-svgrepo-com", R"svg(<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor"  version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink"  width="800px"
	 height="800px" viewBox="0 0 512 512" enable-background="new 0 0 512 512" xml:space="preserve">

<g id="3e91140ac1bfb9903b91c1b0ca09020a">

<path display="inline" d="M400.264,168.995c1.028-6.661,1.993-13.04,2.114-20.066c-16.78-2.065-36.036-1.596-43.639,9.272
		c-14.791,21.138,14.097,72.94,26.432,95.061c3.17,5.683,5.461,9.789,6.514,12.347c1.166,2.831,2.44,5.385,3.755,7.728
		c8.487-18.056,6.297-36.352,4.163-54.124c-1.057-8.799-2.154-17.898-1.867-26.649C398.021,183.57,399.163,176.16,400.264,168.995z
		 M386.131,164.883c-1.363,1.436-4.243,3.927-8.172,4.472c-0.505,0.07-1.021,0.106-1.533,0.106c-5.792,0-10.79-4.52-11.205-7.45
		c-0.476-3.535,5.292-6.224,11.245-7.053c1.303-0.181,2.598-0.273,3.84-0.273c5.144,0,8.552,1.526,8.895,3.98
		C389.422,160.252,388.22,162.692,386.131,164.883z M327.039,33.711c10.601-2.196,25.097-4.245,41.328-3.889
		c41.125,0.91,73.651,16.294,96.683,45.725c17.667,22.573-1.782,125.291-58.099,213.903c-0.553-0.706-1.121-1.42-1.706-2.15
		c-0.233-0.294-0.472-0.593-0.714-0.895c14.553-24.032,11.705-47.806,9.173-68.887c-1.041-8.652-2.021-16.824-1.771-24.498
		c0.259-8.14,1.335-15.115,2.376-21.863c1.278-8.313,2.581-16.915,2.219-27.055c0.27-1.063,0.379-2.319,0.237-3.812
		c-0.915-9.728-12.02-38.829-34.651-65.174C369.738,60.709,351.685,44.587,327.039,33.711z M171.328,292.222
		c1.615,1.686,3.301,3.267,5.044,4.759c-6.948,7.438-22.048,23.891-38.119,43.224c-11.372,13.674-19.226,11.06-21.809,10.196
		c-16.828-5.614-36.354-41.182-53.569-97.587c-14.896-48.802-23.603-97.875-24.29-111.638c-2.176-43.52,8.373-73.852,31.356-90.151
		c37.4-26.526,98.897-10.648,123.607-2.595c-0.357,0.349-0.725,0.677-1.075,1.032c-40.549,40.952-39.587,110.918-39.486,115.193
		c-0.006,1.649,0.133,3.984,0.323,7.199c0.697,11.766,1.996,33.668-1.473,58.473C148.615,253.376,155.718,275.934,171.328,292.222z
		 M397.933,301.079c-8.035,2.319-22.459,7.667-21.144,34.405c-1.057,13.407-8.6,76.218-12.428,98.418
		c-5.054,29.324-15.848,40.246-46.18,46.749c-31.457,6.743-42.586-9.31-49.927-27.654c-4.735-11.847-7.059-65.254-5.413-124.232
		c0.024-0.782-0.089-1.541-0.307-2.251c-0.186-1.375-0.476-2.767-0.879-4.17c-2.452-8.584-8.438-15.759-15.617-18.736
		c-2.247-0.932-5.644-2.335-9.822-2.335c-1.543,0-3.062,0.193-4.558,0.581c1.343-5.526,3.669-11.762,6.189-18.519l1.059-2.843
		c1.19-3.207,2.687-6.526,4.268-10.044c8.545-18.981,20.248-44.984,7.547-103.724c-4.758-22.001-20.647-32.744-44.728-30.25
		c-14.44,1.495-27.647,7.321-34.236,10.661c-1.422,0.72-2.727,1.417-3.941,2.095c1.817-22.136,8.735-63.497,34.751-89.773
		c16.366-16.531,38.163-24.695,64.748-24.256c52.314,0.859,85.861,27.706,104.795,50.078c16.315,19.28,25.148,38.699,28.674,49.173
		c-26.504-2.696-44.546,2.539-53.686,15.607c-19.89,28.429,10.878,83.603,25.665,110.121c2.71,4.862,5.054,9.058,5.788,10.845
		c4.812,11.669,11.048,19.461,15.605,25.148C395.549,297.917,396.9,299.61,397.933,301.079z M248.003,326.252
		c0.581,2.037,1.238,5.901-0.907,8.902c-10.808,15.134-25.686,22.81-44.22,22.81c-6.095,0-12.566-0.848-19.24-2.518
		c-3.904-0.976-7.415-2.67-9.477-4.073c1.721-0.812,4.784-1.912,10.096-3.005c25.71-5.297,29.679-9.031,38.349-20.039
		c1.989-2.524,4.244-5.385,7.367-8.874c2.358-2.638,4.46-3.977,6.246-3.977c1.253,0,2.666,0.54,4.383,1.25
		C243.724,318.027,246.769,321.94,248.003,326.252z M469.358,317.431c0.108-0.024,0.218-0.049,0.322-0.068
		c-3.937,3.683-10.669,8.619-20.224,13.057c-8.213,3.812-21.951,6.671-35.007,7.284c-14.42,0.67-21.761-1.613-23.487-3.025
		c-0.807-16.658,5.396-18.4,11.959-20.244c1.028-0.29,2.037-0.572,3.005-0.911c0.605,0.488,1.267,0.979,1.993,1.456
		C419.508,322.63,440.18,323.453,469.358,317.431z M224.536,278.37l1.055-2.823c1.357-3.658,2.948-7.188,4.633-10.931
		c8.163-18.14,18.326-40.719,6.614-94.891c-2.265-10.464-7.708-16.544-16.642-18.591c-19.575-4.485-47.027,10.223-52.95,15.475
		c0.055,1.186,0.144,2.69,0.244,4.404c0.728,12.258,2.081,35.077-1.583,61.281c-2.616,18.712,3.098,36.972,15.676,50.093
		c9.895,10.325,22.902,16.037,36.275,16.077C219.396,292.156,221.874,285.518,224.536,278.37z M206.106,168.942
		c-0.917-1.216-2.411-3.575-2.109-5.707c0.428-3.124,4.188-4.998,10.058-4.998c1.301,0,2.648,0.097,4.008,0.287
		c6.336,0.881,12.875,3.882,12.245,8.386c-0.474,3.412-5.877,8.472-12.366,8.472c-0.567,0-1.14-0.04-1.698-0.119
		C212.492,174.745,208.7,172.381,206.106,168.942z M503.183,308.227c-2.113-9.584-10.793-20.777-28.299-20.777
		c-3.513,0-7.32,0.432-11.641,1.327c-8.301,1.71-15.706,2.775-22.071,3.171c22.293-38.223,40.23-81.256,50.633-121.528
		c9.628-37.277,16.912-87.071-3.054-112.586C459.399,20.326,416.475,0.501,364.616,0.501c-29.228,0-53.098,6.53-60.326,8.74
		c-11.398-1.988-23.668-3.102-36.482-3.31c-0.855-0.014-1.702-0.02-2.546-0.02c-23.059,0-43.723,5.237-61.434,15.567
		c-16.13-5.401-47.269-14.147-79.702-14.147c-36.481,0-65.214,10.757-85.396,31.976C16.385,62.799,6.212,97.864,8.497,143.529
		c0.956,19.149,11.989,76.369,27.785,126.056c24.404,76.774,51.286,112.527,84.595,112.527c10.429,0,20.326-3.65,29.448-10.854
		c7.999,8.717,19.609,12.653,25.994,14.251c9.058,2.267,17.989,3.416,26.547,3.416c11.437,0,22.208-1.997,32.061-5.941
		c0.064,2.142,0.123,4.171,0.173,6.002c0.117,4.162,0.23,8.244,0.384,12.061c1.04,25.951,2.815,46.143,8.089,60.381
		c0.278,0.747,0.649,1.888,1.053,3.122c2.305,7.067,5.788,17.739,13.801,27.428c10.563,12.771,25.616,19.522,43.525,19.522
		c5.72,0,11.847-0.689,18.215-2.057c19.248-4.122,41.114-10.423,57.019-33.067c14.911-21.232,22.144-53,23.454-103.008l0.577-4.921
		l2.775,0.227c1.758,0.08,3.577,0.116,5.408,0.116c18.672,0,39.363-4.005,52.723-10.208
		C473.222,353.426,508.999,334.565,503.183,308.227z M455.442,343.302c-11.487,5.332-29.791,8.704-45.659,8.704
		c-7.611,0-14.649-0.782-20.119-2.484c-2.59,23.806-8.096,68.198-11.302,86.801c-6.002,34.785-21.503,50.564-57.204,58.22
		c-6.078,1.299-11.604,1.88-16.622,1.884c-34.212,0.004-45.038-27.077-49.469-38.153c-5.413-13.536-7.294-57.953-6.913-103.031
		c-12.282,11.116-27.68,16.929-45.278,16.929c-7.256,0-14.888-0.988-22.685-2.94c-2.255-0.565-22.035-5.873-21.771-18.519
		c0.239-11.572,16.753-14.977,22.969-16.259c21.854-4.502,23.266-6.292,30.056-14.912c1.373-1.746,2.986-3.787,4.953-6.119
		c-0.022-0.258-0.051-0.512-0.066-0.77c-9.8-0.263-19.379-2.876-28.1-7.556c-5.899,6.28-21.91,23.564-39.055,44.191
		c-10.323,12.411-20.244,16.058-28.337,16.058c-3.3,0.004-6.295-0.604-8.893-1.473c-22.182-7.396-43.266-43.368-62.66-106.91
		c-14.736-48.277-24.072-98.686-24.891-115.076c-2.437-48.707,10.132-83.181,37.356-102.47
		c48.744-34.534,127.507-8.015,144.689-1.577c16.941-11.55,37.393-17.215,61.105-16.842c12.92,0.212,24.782,1.874,35.665,4.58
		c5.171-2.269,31.844-10.338,63.378-9.991c45.897,0.488,83.813,18.193,109.647,51.204c17.433,22.279,5.93,76.516,0.169,98.799
		c-11.971,46.351-34.163,96.026-61.14,137.128c4.082,3.001,16.518,7.962,51.218,0.798c11.688-2.412,18.724-0.282,20.909,6.325
		C491.615,322.609,468.342,337.315,455.442,343.302z">

</path>

</g>

</svg>)svg"},
        {"dbs_postgresql", R"svg(<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor"  version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink"  width="800px"
	 height="800px" viewBox="0 0 512 512" enable-background="new 0 0 512 512" xml:space="preserve">

<g id="3e91140ac1bfb9903b91c1b0ca09020a">

<path display="inline" d="M400.264,168.995c1.028-6.661,1.993-13.04,2.114-20.066c-16.78-2.065-36.036-1.596-43.639,9.272
		c-14.791,21.138,14.097,72.94,26.432,95.061c3.17,5.683,5.461,9.789,6.514,12.347c1.166,2.831,2.44,5.385,3.755,7.728
		c8.487-18.056,6.297-36.352,4.163-54.124c-1.057-8.799-2.154-17.898-1.867-26.649C398.021,183.57,399.163,176.16,400.264,168.995z
		 M386.131,164.883c-1.363,1.436-4.243,3.927-8.172,4.472c-0.505,0.07-1.021,0.106-1.533,0.106c-5.792,0-10.79-4.52-11.205-7.45
		c-0.476-3.535,5.292-6.224,11.245-7.053c1.303-0.181,2.598-0.273,3.84-0.273c5.144,0,8.552,1.526,8.895,3.98
		C389.422,160.252,388.22,162.692,386.131,164.883z M327.039,33.711c10.601-2.196,25.097-4.245,41.328-3.889
		c41.125,0.91,73.651,16.294,96.683,45.725c17.667,22.573-1.782,125.291-58.099,213.903c-0.553-0.706-1.121-1.42-1.706-2.15
		c-0.233-0.294-0.472-0.593-0.714-0.895c14.553-24.032,11.705-47.806,9.173-68.887c-1.041-8.652-2.021-16.824-1.771-24.498
		c0.259-8.14,1.335-15.115,2.376-21.863c1.278-8.313,2.581-16.915,2.219-27.055c0.27-1.063,0.379-2.319,0.237-3.812
		c-0.915-9.728-12.02-38.829-34.651-65.174C369.738,60.709,351.685,44.587,327.039,33.711z M171.328,292.222
		c1.615,1.686,3.301,3.267,5.044,4.759c-6.948,7.438-22.048,23.891-38.119,43.224c-11.372,13.674-19.226,11.06-21.809,10.196
		c-16.828-5.614-36.354-41.182-53.569-97.587c-14.896-48.802-23.603-97.875-24.29-111.638c-2.176-43.52,8.373-73.852,31.356-90.151
		c37.4-26.526,98.897-10.648,123.607-2.595c-0.357,0.349-0.725,0.677-1.075,1.032c-40.549,40.952-39.587,110.918-39.486,115.193
		c-0.006,1.649,0.133,3.984,0.323,7.199c0.697,11.766,1.996,33.668-1.473,58.473C148.615,253.376,155.718,275.934,171.328,292.222z
		 M397.933,301.079c-8.035,2.319-22.459,7.667-21.144,34.405c-1.057,13.407-8.6,76.218-12.428,98.418
		c-5.054,29.324-15.848,40.246-46.18,46.749c-31.457,6.743-42.586-9.31-49.927-27.654c-4.735-11.847-7.059-65.254-5.413-124.232
		c0.024-0.782-0.089-1.541-0.307-2.251c-0.186-1.375-0.476-2.767-0.879-4.17c-2.452-8.584-8.438-15.759-15.617-18.736
		c-2.247-0.932-5.644-2.335-9.822-2.335c-1.543,0-3.062,0.193-4.558,0.581c1.343-5.526,3.669-11.762,6.189-18.519l1.059-2.843
		c1.19-3.207,2.687-6.526,4.268-10.044c8.545-18.981,20.248-44.984,7.547-103.724c-4.758-22.001-20.647-32.744-44.728-30.25
		c-14.44,1.495-27.647,7.321-34.236,10.661c-1.422,0.72-2.727,1.417-3.941,2.095c1.817-22.136,8.735-63.497,34.751-89.773
		c16.366-16.531,38.163-24.695,64.748-24.256c52.314,0.859,85.861,27.706,104.795,50.078c16.315,19.28,25.148,38.699,28.674,49.173
		c-26.504-2.696-44.546,2.539-53.686,15.607c-19.89,28.429,10.878,83.603,25.665,110.121c2.71,4.862,5.054,9.058,5.788,10.845
		c4.812,11.669,11.048,19.461,15.605,25.148C395.549,297.917,396.9,299.61,397.933,301.079z M248.003,326.252
		c0.581,2.037,1.238,5.901-0.907,8.902c-10.808,15.134-25.686,22.81-44.22,22.81c-6.095,0-12.566-0.848-19.24-2.518
		c-3.904-0.976-7.415-2.67-9.477-4.073c1.721-0.812,4.784-1.912,10.096-3.005c25.71-5.297,29.679-9.031,38.349-20.039
		c1.989-2.524,4.244-5.385,7.367-8.874c2.358-2.638,4.46-3.977,6.246-3.977c1.253,0,2.666,0.54,4.383,1.25
		C243.724,318.027,246.769,321.94,248.003,326.252z M469.358,317.431c0.108-0.024,0.218-0.049,0.322-0.068
		c-3.937,3.683-10.669,8.619-20.224,13.057c-8.213,3.812-21.951,6.671-35.007,7.284c-14.42,0.67-21.761-1.613-23.487-3.025
		c-0.807-16.658,5.396-18.4,11.959-20.244c1.028-0.29,2.037-0.572,3.005-0.911c0.605,0.488,1.267,0.979,1.993,1.456
		C419.508,322.63,440.18,323.453,469.358,317.431z M224.536,278.37l1.055-2.823c1.357-3.658,2.948-7.188,4.633-10.931
		c8.163-18.14,18.326-40.719,6.614-94.891c-2.265-10.464-7.708-16.544-16.642-18.591c-19.575-4.485-47.027,10.223-52.95,15.475
		c0.055,1.186,0.144,2.69,0.244,4.404c0.728,12.258,2.081,35.077-1.583,61.281c-2.616,18.712,3.098,36.972,15.676,50.093
		c9.895,10.325,22.902,16.037,36.275,16.077C219.396,292.156,221.874,285.518,224.536,278.37z M206.106,168.942
		c-0.917-1.216-2.411-3.575-2.109-5.707c0.428-3.124,4.188-4.998,10.058-4.998c1.301,0,2.648,0.097,4.008,0.287
		c6.336,0.881,12.875,3.882,12.245,8.386c-0.474,3.412-5.877,8.472-12.366,8.472c-0.567,0-1.14-0.04-1.698-0.119
		C212.492,174.745,208.7,172.381,206.106,168.942z M503.183,308.227c-2.113-9.584-10.793-20.777-28.299-20.777
		c-3.513,0-7.32,0.432-11.641,1.327c-8.301,1.71-15.706,2.775-22.071,3.171c22.293-38.223,40.23-81.256,50.633-121.528
		c9.628-37.277,16.912-87.071-3.054-112.586C459.399,20.326,416.475,0.501,364.616,0.501c-29.228,0-53.098,6.53-60.326,8.74
		c-11.398-1.988-23.668-3.102-36.482-3.31c-0.855-0.014-1.702-0.02-2.546-0.02c-23.059,0-43.723,5.237-61.434,15.567
		c-16.13-5.401-47.269-14.147-79.702-14.147c-36.481,0-65.214,10.757-85.396,31.976C16.385,62.799,6.212,97.864,8.497,143.529
		c0.956,19.149,11.989,76.369,27.785,126.056c24.404,76.774,51.286,112.527,84.595,112.527c10.429,0,20.326-3.65,29.448-10.854
		c7.999,8.717,19.609,12.653,25.994,14.251c9.058,2.267,17.989,3.416,26.547,3.416c11.437,0,22.208-1.997,32.061-5.941
		c0.064,2.142,0.123,4.171,0.173,6.002c0.117,4.162,0.23,8.244,0.384,12.061c1.04,25.951,2.815,46.143,8.089,60.381
		c0.278,0.747,0.649,1.888,1.053,3.122c2.305,7.067,5.788,17.739,13.801,27.428c10.563,12.771,25.616,19.522,43.525,19.522
		c5.72,0,11.847-0.689,18.215-2.057c19.248-4.122,41.114-10.423,57.019-33.067c14.911-21.232,22.144-53,23.454-103.008l0.577-4.921
		l2.775,0.227c1.758,0.08,3.577,0.116,5.408,0.116c18.672,0,39.363-4.005,52.723-10.208
		C473.222,353.426,508.999,334.565,503.183,308.227z M455.442,343.302c-11.487,5.332-29.791,8.704-45.659,8.704
		c-7.611,0-14.649-0.782-20.119-2.484c-2.59,23.806-8.096,68.198-11.302,86.801c-6.002,34.785-21.503,50.564-57.204,58.22
		c-6.078,1.299-11.604,1.88-16.622,1.884c-34.212,0.004-45.038-27.077-49.469-38.153c-5.413-13.536-7.294-57.953-6.913-103.031
		c-12.282,11.116-27.68,16.929-45.278,16.929c-7.256,0-14.888-0.988-22.685-2.94c-2.255-0.565-22.035-5.873-21.771-18.519
		c0.239-11.572,16.753-14.977,22.969-16.259c21.854-4.502,23.266-6.292,30.056-14.912c1.373-1.746,2.986-3.787,4.953-6.119
		c-0.022-0.258-0.051-0.512-0.066-0.77c-9.8-0.263-19.379-2.876-28.1-7.556c-5.899,6.28-21.91,23.564-39.055,44.191
		c-10.323,12.411-20.244,16.058-28.337,16.058c-3.3,0.004-6.295-0.604-8.893-1.473c-22.182-7.396-43.266-43.368-62.66-106.91
		c-14.736-48.277-24.072-98.686-24.891-115.076c-2.437-48.707,10.132-83.181,37.356-102.47
		c48.744-34.534,127.507-8.015,144.689-1.577c16.941-11.55,37.393-17.215,61.105-16.842c12.92,0.212,24.782,1.874,35.665,4.58
		c5.171-2.269,31.844-10.338,63.378-9.991c45.897,0.488,83.813,18.193,109.647,51.204c17.433,22.279,5.93,76.516,0.169,98.799
		c-11.971,46.351-34.163,96.026-61.14,137.128c4.082,3.001,16.518,7.962,51.218,0.798c11.688-2.412,18.724-0.282,20.909,6.325
		C491.615,322.609,468.342,337.315,455.442,343.302z">

</path>

</g>

</svg>)svg"},
        {"delete-forever-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg xmlns="http://www.w3.org/2000/svg" fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24"><path d="M6 19c0 1.1.9 2 2 2h8c1.1 0 2-.9 2-2V7H6v12zm2.46-7.12l1.41-1.41L12 12.59l2.12-2.12 1.41 1.41L13.41 14l2.12 2.12-1.41 1.41L12 15.41l-2.12 2.12-1.41-1.41L10.59 14l-2.13-2.12zM15.5 4l-1-1h-5l-1 1H5v2h14V4z"/></svg>)svg"},
        {"delete-forever-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg xmlns="http://www.w3.org/2000/svg" fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24"><path d="M6 19c0 1.1.9 2 2 2h8c1.1 0 2-.9 2-2V7H6v12zm2.46-7.12l1.41-1.41L12 12.59l2.12-2.12 1.41 1.41L13.41 14l2.12 2.12-1.41 1.41L12 15.41l-2.12 2.12-1.41-1.41L10.59 14l-2.13-2.12zM15.5 4l-1-1h-5l-1 1H5v2h14V4z"/></svg>)svg"},
        {"delete_forever", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg xmlns="http://www.w3.org/2000/svg" fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24"><path d="M6 19c0 1.1.9 2 2 2h8c1.1 0 2-.9 2-2V7H6v12zm2.46-7.12l1.41-1.41L12 12.59l2.12-2.12 1.41 1.41L13.41 14l2.12 2.12-1.41 1.41L12 15.41l-2.12 2.12-1.41-1.41L10.59 14l-2.13-2.12zM15.5 4l-1-1h-5l-1 1H5v2h14V4z"/></svg>)svg"},
        {"diskette-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>diskette</title>
<path d="M0 26.016q0 1.92 1.12 3.488t2.88 2.144v-11.648q0-0.832 0.576-1.408t1.44-0.576h20q0.8 0 1.408 0.576t0.576 1.408v11.648q1.76-0.608 2.88-2.144t1.12-3.488v-20q0-1.952-1.12-3.488t-2.88-2.144v11.616q0 0.832-0.576 1.44t-1.408 0.576h-20q-0.832 0-1.44-0.576t-0.576-1.44v-11.616q-1.76 0.64-2.88 2.144t-1.12 3.488v20zM6.016 30.016q0 0.832 0.576 1.408t1.408 0.576h16q0.832 0 1.408-0.576t0.608-1.408v-8q0-0.832-0.608-1.408t-1.408-0.608h-16q-0.832 0-1.408 0.608t-0.576 1.408v8zM10.016 10.016q0 0.832 0.576 1.408t1.408 0.576h12q0.832 0 1.408-0.576t0.608-1.408v-8q0-0.832-0.608-1.408t-1.408-0.608h-12q-0.832 0-1.408 0.608t-0.576 1.408v8zM20 8v-4q0-0.832 0.576-1.408t1.44-0.576 1.408 0.576 0.576 1.408v4q0 0.832-0.576 1.44t-1.408 0.576-1.44-0.576-0.576-1.44z"></path>
</svg>)svg"},
        {"diskette-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>diskette</title>
<path d="M0 26.016q0 1.92 1.12 3.488t2.88 2.144v-11.648q0-0.832 0.576-1.408t1.44-0.576h20q0.8 0 1.408 0.576t0.576 1.408v11.648q1.76-0.608 2.88-2.144t1.12-3.488v-20q0-1.952-1.12-3.488t-2.88-2.144v11.616q0 0.832-0.576 1.44t-1.408 0.576h-20q-0.832 0-1.44-0.576t-0.576-1.44v-11.616q-1.76 0.64-2.88 2.144t-1.12 3.488v20zM6.016 30.016q0 0.832 0.576 1.408t1.408 0.576h16q0.832 0 1.408-0.576t0.608-1.408v-8q0-0.832-0.608-1.408t-1.408-0.608h-16q-0.832 0-1.408 0.608t-0.576 1.408v8zM10.016 10.016q0 0.832 0.576 1.408t1.408 0.576h12q0.832 0 1.408-0.576t0.608-1.408v-8q0-0.832-0.608-1.408t-1.408-0.608h-12q-0.832 0-1.408 0.608t-0.576 1.408v8zM20 8v-4q0-0.832 0.576-1.408t1.44-0.576 1.408 0.576 0.576 1.408v4q0 0.832-0.576 1.44t-1.408 0.576-1.44-0.576-0.576-1.44z"></path>
</svg>)svg"},
        {"diskette", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>diskette</title>
<path d="M0 26.016q0 1.92 1.12 3.488t2.88 2.144v-11.648q0-0.832 0.576-1.408t1.44-0.576h20q0.8 0 1.408 0.576t0.576 1.408v11.648q1.76-0.608 2.88-2.144t1.12-3.488v-20q0-1.952-1.12-3.488t-2.88-2.144v11.616q0 0.832-0.576 1.44t-1.408 0.576h-20q-0.832 0-1.44-0.576t-0.576-1.44v-11.616q-1.76 0.64-2.88 2.144t-1.12 3.488v20zM6.016 30.016q0 0.832 0.576 1.408t1.408 0.576h16q0.832 0 1.408-0.576t0.608-1.408v-8q0-0.832-0.608-1.408t-1.408-0.608h-16q-0.832 0-1.408 0.608t-0.576 1.408v8zM10.016 10.016q0 0.832 0.576 1.408t1.408 0.576h12q0.832 0 1.408-0.576t0.608-1.408v-8q0-0.832-0.608-1.408t-1.408-0.608h-12q-0.832 0-1.408 0.608t-0.576 1.408v8zM20 8v-4q0-0.832 0.576-1.408t1.44-0.576 1.408 0.576 0.576 1.408v4q0 0.832-0.576 1.44t-1.408 0.576-1.44-0.576-0.576-1.44z"></path>
</svg>)svg"},
        {"divide-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg"><path d="M16,4V6a1,1,0,0,1-2,0V5H5v9H6a1,1,0,0,1,0,2H4a1,1,0,0,1-1-1V4A1,1,0,0,1,4,3H15A1,1,0,0,1,16,4ZM10,19V18a1,1,0,0,0-2,0v2a1,1,0,0,0,1,1H20a1,1,0,0,0,1-1V9a1,1,0,0,0-1-1H18a1,1,0,0,0,0,2h1v9Zm4-4a1,1,0,0,0,1-1V10a1,1,0,0,0-1-1H10a1,1,0,0,0-1,1v4a1,1,0,0,0,1,1Z"/></svg>)svg"},
        {"divide-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg"><path d="M16,4V6a1,1,0,0,1-2,0V5H5v9H6a1,1,0,0,1,0,2H4a1,1,0,0,1-1-1V4A1,1,0,0,1,4,3H15A1,1,0,0,1,16,4ZM10,19V18a1,1,0,0,0-2,0v2a1,1,0,0,0,1,1H20a1,1,0,0,0,1-1V9a1,1,0,0,0-1-1H18a1,1,0,0,0,0,2h1v9Zm4-4a1,1,0,0,0,1-1V10a1,1,0,0,0-1-1H10a1,1,0,0,0-1,1v4a1,1,0,0,0,1,1Z"/></svg>)svg"},
        {"divide", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg"><path d="M16,4V6a1,1,0,0,1-2,0V5H5v9H6a1,1,0,0,1,0,2H4a1,1,0,0,1-1-1V4A1,1,0,0,1,4,3H15A1,1,0,0,1,16,4ZM10,19V18a1,1,0,0,0-2,0v2a1,1,0,0,0,1,1H20a1,1,0,0,0,1-1V9a1,1,0,0,0-1-1H18a1,1,0,0,0,0,2h1v9Zm4-4a1,1,0,0,0,1-1V10a1,1,0,0,0-1-1H10a1,1,0,0,0-1,1v4a1,1,0,0,0,1,1Z"/></svg>)svg"},
        {"exit-full-screen-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 1920 1920" xmlns="http://www.w3.org/2000/svg">
    <path d="M884.311 1035.689v696.318H675.186v-339.162L147.926 1920 0 1772.074l527.26-527.155H187.889v-209.23H884.31ZM1772.116 0l147.926 147.926-527.155 527.155h339.162v209.335h-696.423V187.889h209.335v339.266L1772.116 0Z" fill-rule="evenodd"/>
</svg>)svg"},
        {"exit-full-screen-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 1920 1920" xmlns="http://www.w3.org/2000/svg">
    <path d="M884.311 1035.689v696.318H675.186v-339.162L147.926 1920 0 1772.074l527.26-527.155H187.889v-209.23H884.31ZM1772.116 0l147.926 147.926-527.155 527.155h339.162v209.335h-696.423V187.889h209.335v339.266L1772.116 0Z" fill-rule="evenodd"/>
</svg>)svg"},
        {"exit_full_screen", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 1920 1920" xmlns="http://www.w3.org/2000/svg">
    <path d="M884.311 1035.689v696.318H675.186v-339.162L147.926 1920 0 1772.074l527.26-527.155H187.889v-209.23H884.31ZM1772.116 0l147.926 147.926-527.155 527.155h339.162v209.335h-696.423V187.889h209.335v339.266L1772.116 0Z" fill-rule="evenodd"/>
</svg>)svg"},
        {"explore-filled-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="UTF-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg width="800px" height="800px" viewBox="0 0 512 512" version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
    <title>explore-filled</title>
    <g id="Page-1" stroke="none" stroke-width="1" fill="none" fill-rule="evenodd">
        <g id="icon" fill="currentColor" transform="translate(42.666667, 42.666667)">
            <path d="M250.929,70.72 L314.314,232.128 L283.413333,241.216 L346.66944,409.38432 L306.71936,424.365653 L242.368,253.290667 L213.44,261.781333 L153.778347,424.38016 L114.561493,407.572907 L162.56,276.757333 L37.7361067,313.4784 L-7.10542736e-15,238.005973 L250.929,70.72 Z M357.009067,0 L440.57856,194.9952 L355.407,220.043 L287.257,46.501 L357.009067,0 Z" id="Combined-Shape">

</path>
        </g>
    </g>
</svg>)svg"},
        {"explore-filled-svgrepo-com", R"svg(<?xml version="1.0" encoding="UTF-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg width="800px" height="800px" viewBox="0 0 512 512" version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
    <title>explore-filled</title>
    <g id="Page-1" stroke="none" stroke-width="1" fill="none" fill-rule="evenodd">
        <g id="icon" fill="currentColor" transform="translate(42.666667, 42.666667)">
            <path d="M250.929,70.72 L314.314,232.128 L283.413333,241.216 L346.66944,409.38432 L306.71936,424.365653 L242.368,253.290667 L213.44,261.781333 L153.778347,424.38016 L114.561493,407.572907 L162.56,276.757333 L37.7361067,313.4784 L-7.10542736e-15,238.005973 L250.929,70.72 Z M357.009067,0 L440.57856,194.9952 L355.407,220.043 L287.257,46.501 L357.009067,0 Z" id="Combined-Shape">

</path>
        </g>
    </g>
</svg>)svg"},
        {"explore_filled", R"svg(<?xml version="1.0" encoding="UTF-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg width="800px" height="800px" viewBox="0 0 512 512" version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
    <title>explore-filled</title>
    <g id="Page-1" stroke="none" stroke-width="1" fill="none" fill-rule="evenodd">
        <g id="icon" fill="currentColor" transform="translate(42.666667, 42.666667)">
            <path d="M250.929,70.72 L314.314,232.128 L283.413333,241.216 L346.66944,409.38432 L306.71936,424.365653 L242.368,253.290667 L213.44,261.781333 L153.778347,424.38016 L114.561493,407.572907 L162.56,276.757333 L37.7361067,313.4784 L-7.10542736e-15,238.005973 L250.929,70.72 Z M357.009067,0 L440.57856,194.9952 L355.407,220.043 L287.257,46.501 L357.009067,0 Z" id="Combined-Shape">

</path>
        </g>
    </g>
</svg>)svg"},
        {"extension-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg 
  width="24"
  height="24"
  viewBox="0 0 24 24"
  fill="none"
  xmlns="http://www.w3.org/2000/svg"
>
  <path
    fill-rule="evenodd"
    clip-rule="evenodd"
    d="M13 3H21V11H13V3ZM15 5H19V9H15V5Z"
    fill="currentColor"
  />
  <path
    fill-rule="evenodd"
    clip-rule="evenodd"
    d="M17 21V13H11V7H3V21H17ZM9 9H5V13H9V9ZM5 19L5 15H9V19H5ZM11 19V15H15V19H11Z"
    fill="currentColor"
  />
</svg>)svg"},
        {"extension-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg 
  width="24"
  height="24"
  viewBox="0 0 24 24"
  fill="none"
  xmlns="http://www.w3.org/2000/svg"
>
  <path
    fill-rule="evenodd"
    clip-rule="evenodd"
    d="M13 3H21V11H13V3ZM15 5H19V9H15V5Z"
    fill="currentColor"
  />
  <path
    fill-rule="evenodd"
    clip-rule="evenodd"
    d="M17 21V13H11V7H3V21H17ZM9 9H5V13H9V9ZM5 19L5 15H9V19H5ZM11 19V15H15V19H11Z"
    fill="currentColor"
  />
</svg>)svg"},
        {"extension", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg 
  width="24"
  height="24"
  viewBox="0 0 24 24"
  fill="none"
  xmlns="http://www.w3.org/2000/svg"
>
  <path
    fill-rule="evenodd"
    clip-rule="evenodd"
    d="M13 3H21V11H13V3ZM15 5H19V9H15V5Z"
    fill="currentColor"
  />
  <path
    fill-rule="evenodd"
    clip-rule="evenodd"
    d="M17 21V13H11V7H3V21H17ZM9 9H5V13H9V9ZM5 19L5 15H9V19H5ZM11 19V15H15V19H11Z"
    fill="currentColor"
  />
</svg>)svg"},
        {"eye-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>eye</title>
<path d="M0 16q0.064 0.128 0.16 0.352t0.48 0.928 0.832 1.344 1.248 1.536 1.664 1.696 2.144 1.568 2.624 1.344 3.136 0.896 3.712 0.352 3.712-0.352 3.168-0.928 2.592-1.312 2.144-1.6 1.664-1.632 1.248-1.6 0.832-1.312 0.48-0.928l0.16-0.352q-0.032-0.128-0.16-0.352t-0.48-0.896-0.832-1.344-1.248-1.568-1.664-1.664-2.144-1.568-2.624-1.344-3.136-0.896-3.712-0.352-3.712 0.352-3.168 0.896-2.592 1.344-2.144 1.568-1.664 1.664-1.248 1.568-0.832 1.344-0.48 0.928zM10.016 16q0-2.464 1.728-4.224t4.256-1.76 4.256 1.76 1.76 4.224-1.76 4.256-4.256 1.76-4.256-1.76-1.728-4.256zM12 16q0 1.664 1.184 2.848t2.816 1.152 2.816-1.152 1.184-2.848-1.184-2.816-2.816-1.184-2.816 1.184l2.816 2.816h-4z"></path>
</svg>)svg"},
        {"eye-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>eye</title>
<path d="M0 16q0.064 0.128 0.16 0.352t0.48 0.928 0.832 1.344 1.248 1.536 1.664 1.696 2.144 1.568 2.624 1.344 3.136 0.896 3.712 0.352 3.712-0.352 3.168-0.928 2.592-1.312 2.144-1.6 1.664-1.632 1.248-1.6 0.832-1.312 0.48-0.928l0.16-0.352q-0.032-0.128-0.16-0.352t-0.48-0.896-0.832-1.344-1.248-1.568-1.664-1.664-2.144-1.568-2.624-1.344-3.136-0.896-3.712-0.352-3.712 0.352-3.168 0.896-2.592 1.344-2.144 1.568-1.664 1.664-1.248 1.568-0.832 1.344-0.48 0.928zM10.016 16q0-2.464 1.728-4.224t4.256-1.76 4.256 1.76 1.76 4.224-1.76 4.256-4.256 1.76-4.256-1.76-1.728-4.256zM12 16q0 1.664 1.184 2.848t2.816 1.152 2.816-1.152 1.184-2.848-1.184-2.816-2.816-1.184-2.816 1.184l2.816 2.816h-4z"></path>
</svg>)svg"},
        {"file-search-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg" id="file-search" class="icon glyph"><path d="M18,7.41V9.26A6.89,6.89,0,0,0,14,8a7,7,0,0,0-3.6,13H4a2,2,0,0,1-2-2V4A2,2,0,0,1,4,2h8.59A2,2,0,0,1,14,2.59L17.41,6A2,2,0,0,1,18,7.41Zm3.76,14.24a1,1,0,0,1-1.41.11l-3.3-2.83A5,5,0,1,1,19,15a4.9,4.9,0,0,1-.65,2.41l3.3,2.83A1,1,0,0,1,21.76,21.65ZM14,18a3,3,0,1,0-3-3A3,3,0,0,0,14,18Z"></path></svg>)svg"},
        {"file-search-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg" id="file-search" class="icon glyph"><path d="M18,7.41V9.26A6.89,6.89,0,0,0,14,8a7,7,0,0,0-3.6,13H4a2,2,0,0,1-2-2V4A2,2,0,0,1,4,2h8.59A2,2,0,0,1,14,2.59L17.41,6A2,2,0,0,1,18,7.41Zm3.76,14.24a1,1,0,0,1-1.41.11l-3.3-2.83A5,5,0,1,1,19,15a4.9,4.9,0,0,1-.65,2.41l3.3,2.83A1,1,0,0,1,21.76,21.65ZM14,18a3,3,0,1,0-3-3A3,3,0,0,0,14,18Z"></path></svg>)svg"},
        {"file_search", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg" id="file-search" class="icon glyph"><path d="M18,7.41V9.26A6.89,6.89,0,0,0,14,8a7,7,0,0,0-3.6,13H4a2,2,0,0,1-2-2V4A2,2,0,0,1,4,2h8.59A2,2,0,0,1,14,2.59L17.41,6A2,2,0,0,1,18,7.41Zm3.76,14.24a1,1,0,0,1-1.41.11l-3.3-2.83A5,5,0,1,1,19,15a4.9,4.9,0,0,1-.65,2.41l3.3,2.83A1,1,0,0,1,21.76,21.65ZM14,18a3,3,0,1,0-3-3A3,3,0,0,0,14,18Z"></path></svg>)svg"},
        {"filter-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 1920 1920" xmlns="http://www.w3.org/2000/svg">
    <path d="M1672.853 0 1171.84 640H748.053L426.56 213.333h637.227L1241.173 0H0l746.667 991.147V1600l426.56 320V991.147L1920 0z" fill-rule="evenodd"/>
</svg>)svg"},
        {"filter-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 1920 1920" xmlns="http://www.w3.org/2000/svg">
    <path d="M1672.853 0 1171.84 640H748.053L426.56 213.333h637.227L1241.173 0H0l746.667 991.147V1600l426.56 320V991.147L1920 0z" fill-rule="evenodd"/>
</svg>)svg"},
        {"folder-add-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg" id="folder-add" class="icon glyph"><path d="M22,7v7.77A3,3,0,0,0,20,14H19V13a3,3,0,0,0-6,0v1H12a3,3,0,0,0-2.23,5H4a2,2,0,0,1-2-2V4A2,2,0,0,1,4,2H9a2,2,0,0,1,1.41.59L12.83,5H20A2,2,0,0,1,22,7Zm-2,9H17V13a1,1,0,0,0-2,0v3H12a1,1,0,0,0,0,2h3v3a1,1,0,0,0,2,0V18h3a1,1,0,0,0,0-2Z"></path></svg>)svg"},
        {"folder-add-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg" id="folder-add" class="icon glyph"><path d="M22,7v7.77A3,3,0,0,0,20,14H19V13a3,3,0,0,0-6,0v1H12a3,3,0,0,0-2.23,5H4a2,2,0,0,1-2-2V4A2,2,0,0,1,4,2H9a2,2,0,0,1,1.41.59L12.83,5H20A2,2,0,0,1,22,7Zm-2,9H17V13a1,1,0,0,0-2,0v3H12a1,1,0,0,0,0,2h3v3a1,1,0,0,0,2,0V18h3a1,1,0,0,0,0-2Z"></path></svg>)svg"},
        {"folder_add", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg" id="folder-add" class="icon glyph"><path d="M22,7v7.77A3,3,0,0,0,20,14H19V13a3,3,0,0,0-6,0v1H12a3,3,0,0,0-2.23,5H4a2,2,0,0,1-2-2V4A2,2,0,0,1,4,2H9a2,2,0,0,1,1.41.59L12.83,5H20A2,2,0,0,1,22,7Zm-2,9H17V13a1,1,0,0,0-2,0v3H12a1,1,0,0,0,0,2h3v3a1,1,0,0,0,2,0V18h3a1,1,0,0,0,0-2Z"></path></svg>)svg"},
        {"folder-block-svgrepo-com (1).svg", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg" id="folder-block" class="icon glyph"><path d="M20.28,11.8l0-.05-.05,0a6,6,0,0,0-8.48,8.48l0,.05.05,0a6,6,0,0,0,8.48-8.48ZM16,12a4,4,0,0,1,2,.57L12.57,18A4,4,0,0,1,16,12Zm0,8a4,4,0,0,1-2-.57L19.43,14A4,4,0,0,1,16,20ZM22,7v3.73a.69.69,0,0,0-.11-.13l0-.06-.43-.43-.05,0A7.93,7.93,0,0,0,16,8,8,8,0,0,0,8.58,19H4a2,2,0,0,1-2-2V4A2,2,0,0,1,4,2H9a2,2,0,0,1,1.41.59L12.83,5H20A2,2,0,0,1,22,7Z"></path></svg>)svg"},
        {"folder-block-svgrepo-com (1)", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg" id="folder-block" class="icon glyph"><path d="M20.28,11.8l0-.05-.05,0a6,6,0,0,0-8.48,8.48l0,.05.05,0a6,6,0,0,0,8.48-8.48ZM16,12a4,4,0,0,1,2,.57L12.57,18A4,4,0,0,1,16,12Zm0,8a4,4,0,0,1-2-.57L19.43,14A4,4,0,0,1,16,20ZM22,7v3.73a.69.69,0,0,0-.11-.13l0-.06-.43-.43-.05,0A7.93,7.93,0,0,0,16,8,8,8,0,0,0,8.58,19H4a2,2,0,0,1-2-2V4A2,2,0,0,1,4,2H9a2,2,0,0,1,1.41.59L12.83,5H20A2,2,0,0,1,22,7Z"></path></svg>)svg"},
        {"folder_block", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg" id="folder-block" class="icon glyph"><path d="M20.28,11.8l0-.05-.05,0a6,6,0,0,0-8.48,8.48l0,.05.05,0a6,6,0,0,0,8.48-8.48ZM16,12a4,4,0,0,1,2,.57L12.57,18A4,4,0,0,1,16,12Zm0,8a4,4,0,0,1-2-.57L19.43,14A4,4,0,0,1,16,20ZM22,7v3.73a.69.69,0,0,0-.11-.13l0-.06-.43-.43-.05,0A7.93,7.93,0,0,0,16,8,8,8,0,0,0,8.58,19H4a2,2,0,0,1-2-2V4A2,2,0,0,1,4,2H9a2,2,0,0,1,1.41.59L12.83,5H20A2,2,0,0,1,22,7Z"></path></svg>)svg"},
        {"folder-block-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg" id="folder-block" class="icon glyph"><path d="M20.28,11.8l0-.05-.05,0a6,6,0,0,0-8.48,8.48l0,.05.05,0a6,6,0,0,0,8.48-8.48ZM16,12a4,4,0,0,1,2,.57L12.57,18A4,4,0,0,1,16,12Zm0,8a4,4,0,0,1-2-.57L19.43,14A4,4,0,0,1,16,20ZM22,7v3.73a.69.69,0,0,0-.11-.13l0-.06-.43-.43-.05,0A7.93,7.93,0,0,0,16,8,8,8,0,0,0,8.58,19H4a2,2,0,0,1-2-2V4A2,2,0,0,1,4,2H9a2,2,0,0,1,1.41.59L12.83,5H20A2,2,0,0,1,22,7Z"></path></svg>)svg"},
        {"folder-block-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg" id="folder-block" class="icon glyph"><path d="M20.28,11.8l0-.05-.05,0a6,6,0,0,0-8.48,8.48l0,.05.05,0a6,6,0,0,0,8.48-8.48ZM16,12a4,4,0,0,1,2,.57L12.57,18A4,4,0,0,1,16,12Zm0,8a4,4,0,0,1-2-.57L19.43,14A4,4,0,0,1,16,20ZM22,7v3.73a.69.69,0,0,0-.11-.13l0-.06-.43-.43-.05,0A7.93,7.93,0,0,0,16,8,8,8,0,0,0,8.58,19H4a2,2,0,0,1-2-2V4A2,2,0,0,1,4,2H9a2,2,0,0,1,1.41.59L12.83,5H20A2,2,0,0,1,22,7Z"></path></svg>)svg"},
        {"folder-check-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg" id="folder-check" class="icon glyph"><path d="M15,22a1,1,0,0,1-.71-.29l-3-3a1,1,0,0,1,1.42-1.42L15,19.59l5.29-5.3a1,1,0,0,1,1.42,1.42l-6,6A1,1,0,0,1,15,22ZM20,5H12.83L10.41,2.59A2,2,0,0,0,9,2H4A2,2,0,0,0,2,4V17a2,2,0,0,0,2,2H9.18A2.81,2.81,0,0,1,9,18a3,3,0,0,1,5.12-2.12l.88.88,3.88-3.88A3,3,0,0,1,21,12a2.81,2.81,0,0,1,1,.18V7A2,2,0,0,0,20,5Z"></path></svg>)svg"},
        {"folder-check-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg" id="folder-check" class="icon glyph"><path d="M15,22a1,1,0,0,1-.71-.29l-3-3a1,1,0,0,1,1.42-1.42L15,19.59l5.29-5.3a1,1,0,0,1,1.42,1.42l-6,6A1,1,0,0,1,15,22ZM20,5H12.83L10.41,2.59A2,2,0,0,0,9,2H4A2,2,0,0,0,2,4V17a2,2,0,0,0,2,2H9.18A2.81,2.81,0,0,1,9,18a3,3,0,0,1,5.12-2.12l.88.88,3.88-3.88A3,3,0,0,1,21,12a2.81,2.81,0,0,1,1,.18V7A2,2,0,0,0,20,5Z"></path></svg>)svg"},
        {"folder_check", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg" id="folder-check" class="icon glyph"><path d="M15,22a1,1,0,0,1-.71-.29l-3-3a1,1,0,0,1,1.42-1.42L15,19.59l5.29-5.3a1,1,0,0,1,1.42,1.42l-6,6A1,1,0,0,1,15,22ZM20,5H12.83L10.41,2.59A2,2,0,0,0,9,2H4A2,2,0,0,0,2,4V17a2,2,0,0,0,2,2H9.18A2.81,2.81,0,0,1,9,18a3,3,0,0,1,5.12-2.12l.88.88,3.88-3.88A3,3,0,0,1,21,12a2.81,2.81,0,0,1,1,.18V7A2,2,0,0,0,20,5Z"></path></svg>)svg"},
        {"folder-code-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg" id="folder-code" class="icon glyph"><path d="M20,6H13.41L11,3.59A2,2,0,0,0,9.59,3H4A2,2,0,0,0,2,5V19a2,2,0,0,0,2,2H20a2,2,0,0,0,2-2V8A2,2,0,0,0,20,6Zm-9.29,8.29a1,1,0,0,1,0,1.42,1,1,0,0,1-1.42,0l-2-2a1,1,0,0,1,0-1.42l2-2a1,1,0,0,1,1.42,1.42L9.41,13Zm6-.58-2,2a1,1,0,0,1-1.42,0,1,1,0,0,1,0-1.42L14.59,13l-1.3-1.29a1,1,0,0,1,1.42-1.42l2,2A1,1,0,0,1,16.71,13.71Z"></path></svg>)svg"},
        {"folder-code-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg" id="folder-code" class="icon glyph"><path d="M20,6H13.41L11,3.59A2,2,0,0,0,9.59,3H4A2,2,0,0,0,2,5V19a2,2,0,0,0,2,2H20a2,2,0,0,0,2-2V8A2,2,0,0,0,20,6Zm-9.29,8.29a1,1,0,0,1,0,1.42,1,1,0,0,1-1.42,0l-2-2a1,1,0,0,1,0-1.42l2-2a1,1,0,0,1,1.42,1.42L9.41,13Zm6-.58-2,2a1,1,0,0,1-1.42,0,1,1,0,0,1,0-1.42L14.59,13l-1.3-1.29a1,1,0,0,1,1.42-1.42l2,2A1,1,0,0,1,16.71,13.71Z"></path></svg>)svg"},
        {"folder_code", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg" id="folder-code" class="icon glyph"><path d="M20,6H13.41L11,3.59A2,2,0,0,0,9.59,3H4A2,2,0,0,0,2,5V19a2,2,0,0,0,2,2H20a2,2,0,0,0,2-2V8A2,2,0,0,0,20,6Zm-9.29,8.29a1,1,0,0,1,0,1.42,1,1,0,0,1-1.42,0l-2-2a1,1,0,0,1,0-1.42l2-2a1,1,0,0,1,1.42,1.42L9.41,13Zm6-.58-2,2a1,1,0,0,1-1.42,0,1,1,0,0,1,0-1.42L14.59,13l-1.3-1.29a1,1,0,0,1,1.42-1.42l2,2A1,1,0,0,1,16.71,13.71Z"></path></svg>)svg"},
        {"folder-edit-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg" id="folder-edit" class="icon glyph"><path d="M21.41,16.05l-5.65,5.66a1,1,0,0,1-.71.29H12.22a1,1,0,0,1-1-1V18.17a1.05,1.05,0,0,1,.29-.71l5.66-5.65a2,2,0,0,1,2.83,0l1.41,1.41A2,2,0,0,1,21.41,16.05ZM20,5H12.83L10.41,2.59A2,2,0,0,0,9,2H4A2,2,0,0,0,2,4V17a2,2,0,0,0,2,2H9.22v-.83a3,3,0,0,1,.88-2.12l5.66-5.66a4,4,0,0,1,5.66,0L22,11V7A2,2,0,0,0,20,5Z"></path></svg>)svg"},
        {"folder-edit-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg" id="folder-edit" class="icon glyph"><path d="M21.41,16.05l-5.65,5.66a1,1,0,0,1-.71.29H12.22a1,1,0,0,1-1-1V18.17a1.05,1.05,0,0,1,.29-.71l5.66-5.65a2,2,0,0,1,2.83,0l1.41,1.41A2,2,0,0,1,21.41,16.05ZM20,5H12.83L10.41,2.59A2,2,0,0,0,9,2H4A2,2,0,0,0,2,4V17a2,2,0,0,0,2,2H9.22v-.83a3,3,0,0,1,.88-2.12l5.66-5.66a4,4,0,0,1,5.66,0L22,11V7A2,2,0,0,0,20,5Z"></path></svg>)svg"},
        {"folder_edit", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg" id="folder-edit" class="icon glyph"><path d="M21.41,16.05l-5.65,5.66a1,1,0,0,1-.71.29H12.22a1,1,0,0,1-1-1V18.17a1.05,1.05,0,0,1,.29-.71l5.66-5.65a2,2,0,0,1,2.83,0l1.41,1.41A2,2,0,0,1,21.41,16.05ZM20,5H12.83L10.41,2.59A2,2,0,0,0,9,2H4A2,2,0,0,0,2,4V17a2,2,0,0,0,2,2H9.22v-.83a3,3,0,0,1,.88-2.12l5.66-5.66a4,4,0,0,1,5.66,0L22,11V7A2,2,0,0,0,20,5Z"></path></svg>)svg"},
        {"folder-favorite-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg" id="folder-favorite" class="icon glyph"><path d="M21,17.89,17.21,21.7a1,1,0,0,1-1.42,0L12,17.89a3.45,3.45,0,0,1,4.5-5.2,3.45,3.45,0,0,1,4.5,5.2ZM20,5H12.83L10.41,2.59A2,2,0,0,0,9,2H4A2,2,0,0,0,2,4V17a2,2,0,0,0,2,2h6.31a5.45,5.45,0,0,1,6.19-8.58,5.46,5.46,0,0,1,2.05-.4A5.6,5.6,0,0,1,22,11.22V7A2,2,0,0,0,20,5Z"></path></svg>)svg"},
        {"folder-favorite-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg" id="folder-favorite" class="icon glyph"><path d="M21,17.89,17.21,21.7a1,1,0,0,1-1.42,0L12,17.89a3.45,3.45,0,0,1,4.5-5.2,3.45,3.45,0,0,1,4.5,5.2ZM20,5H12.83L10.41,2.59A2,2,0,0,0,9,2H4A2,2,0,0,0,2,4V17a2,2,0,0,0,2,2h6.31a5.45,5.45,0,0,1,6.19-8.58,5.46,5.46,0,0,1,2.05-.4A5.6,5.6,0,0,1,22,11.22V7A2,2,0,0,0,20,5Z"></path></svg>)svg"},
        {"folder_favorite", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg" id="folder-favorite" class="icon glyph"><path d="M21,17.89,17.21,21.7a1,1,0,0,1-1.42,0L12,17.89a3.45,3.45,0,0,1,4.5-5.2,3.45,3.45,0,0,1,4.5,5.2ZM20,5H12.83L10.41,2.59A2,2,0,0,0,9,2H4A2,2,0,0,0,2,4V17a2,2,0,0,0,2,2h6.31a5.45,5.45,0,0,1,6.19-8.58,5.46,5.46,0,0,1,2.05-.4A5.6,5.6,0,0,1,22,11.22V7A2,2,0,0,0,20,5Z"></path></svg>)svg"},
        {"folder-remove-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg" id="folder-remove" class="icon glyph"><path d="M22,7v7.77A3,3,0,0,0,20,14H12a3,3,0,0,0-2.23,5H4a2,2,0,0,1-2-2V4A2,2,0,0,1,4,2H9a2,2,0,0,1,1.41.59L12.83,5H20A2,2,0,0,1,22,7ZM21,17a1,1,0,0,0-1-1H12a1,1,0,0,0,0,2h8A1,1,0,0,0,21,17Z"></path></svg>)svg"},
        {"folder-remove-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg" id="folder-remove" class="icon glyph"><path d="M22,7v7.77A3,3,0,0,0,20,14H12a3,3,0,0,0-2.23,5H4a2,2,0,0,1-2-2V4A2,2,0,0,1,4,2H9a2,2,0,0,1,1.41.59L12.83,5H20A2,2,0,0,1,22,7ZM21,17a1,1,0,0,0-1-1H12a1,1,0,0,0,0,2h8A1,1,0,0,0,21,17Z"></path></svg>)svg"},
        {"folder_remove", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg" id="folder-remove" class="icon glyph"><path d="M22,7v7.77A3,3,0,0,0,20,14H12a3,3,0,0,0-2.23,5H4a2,2,0,0,1-2-2V4A2,2,0,0,1,4,2H9a2,2,0,0,1,1.41.59L12.83,5H20A2,2,0,0,1,22,7ZM21,17a1,1,0,0,0-1-1H12a1,1,0,0,0,0,2h8A1,1,0,0,0,21,17Z"></path></svg>)svg"},
        {"folder-search-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg" id="folder-search" class="icon glyph"><path d="M21.71,20.29l-2.54-2.54a5,5,0,1,0-1.42,1.42l2.54,2.54a1,1,0,0,0,1.42,0A1,1,0,0,0,21.71,20.29ZM12,15a3,3,0,1,1,3,3A3,3,0,0,1,12,15ZM22,7v8A7,7,0,0,0,8,15a6.89,6.89,0,0,0,1.26,4H4a2,2,0,0,1-2-2V4A2,2,0,0,1,4,2H9a2,2,0,0,1,1.41.59L12.83,5H20A2,2,0,0,1,22,7Z"></path></svg>)svg"},
        {"folder-search-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg" id="folder-search" class="icon glyph"><path d="M21.71,20.29l-2.54-2.54a5,5,0,1,0-1.42,1.42l2.54,2.54a1,1,0,0,0,1.42,0A1,1,0,0,0,21.71,20.29ZM12,15a3,3,0,1,1,3,3A3,3,0,0,1,12,15ZM22,7v8A7,7,0,0,0,8,15a6.89,6.89,0,0,0,1.26,4H4a2,2,0,0,1-2-2V4A2,2,0,0,1,4,2H9a2,2,0,0,1,1.41.59L12.83,5H20A2,2,0,0,1,22,7Z"></path></svg>)svg"},
        {"fork-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>fork</title>
<path d="M2.016 26.016q0-1.92 1.088-3.456t2.912-2.176v-8.736q-1.792-0.608-2.912-2.144t-1.088-3.488q0-2.496 1.728-4.256t4.256-1.76 4.256 1.76 1.76 4.256q0 1.92-1.12 3.488t-2.88 2.144v4.736q0.992-0.384 1.984-0.384h8q0.832 0 1.408-0.576t0.608-1.408v-0.352q-1.792-0.608-2.912-2.176t-1.088-3.456q0-2.496 1.728-4.256t4.256-1.76 4.256 1.76 1.76 4.256q0 1.92-1.12 3.456t-2.88 2.176v0.352q0 2.496-1.76 4.256t-4.256 1.76h-8q-0.864 0-1.44 0.608 1.536 0.736 2.496 2.176t0.96 3.2q0 2.496-1.76 4.256t-4.256 1.76-4.256-1.76-1.728-4.256zM6.016 26.016q0 0.832 0.576 1.44t1.408 0.576 1.408-0.576 0.608-1.44-0.608-1.408-1.408-0.576-1.408 0.576-0.576 1.408zM6.016 6.016q0 0.832 0.576 1.44t1.408 0.576 1.408-0.576 0.608-1.44-0.608-1.408-1.408-0.576-1.408 0.576-0.576 1.408zM22.016 8.032q0 0.832 0.576 1.408t1.408 0.576 1.408-0.576 0.608-1.408-0.608-1.408-1.408-0.608-1.408 0.608-0.576 1.408z"></path>
</svg>)svg"},
        {"fork-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>fork</title>
<path d="M2.016 26.016q0-1.92 1.088-3.456t2.912-2.176v-8.736q-1.792-0.608-2.912-2.144t-1.088-3.488q0-2.496 1.728-4.256t4.256-1.76 4.256 1.76 1.76 4.256q0 1.92-1.12 3.488t-2.88 2.144v4.736q0.992-0.384 1.984-0.384h8q0.832 0 1.408-0.576t0.608-1.408v-0.352q-1.792-0.608-2.912-2.176t-1.088-3.456q0-2.496 1.728-4.256t4.256-1.76 4.256 1.76 1.76 4.256q0 1.92-1.12 3.456t-2.88 2.176v0.352q0 2.496-1.76 4.256t-4.256 1.76h-8q-0.864 0-1.44 0.608 1.536 0.736 2.496 2.176t0.96 3.2q0 2.496-1.76 4.256t-4.256 1.76-4.256-1.76-1.728-4.256zM6.016 26.016q0 0.832 0.576 1.44t1.408 0.576 1.408-0.576 0.608-1.44-0.608-1.408-1.408-0.576-1.408 0.576-0.576 1.408zM6.016 6.016q0 0.832 0.576 1.44t1.408 0.576 1.408-0.576 0.608-1.44-0.608-1.408-1.408-0.576-1.408 0.576-0.576 1.408zM22.016 8.032q0 0.832 0.576 1.408t1.408 0.576 1.408-0.576 0.608-1.408-0.608-1.408-1.408-0.608-1.408 0.608-0.576 1.408z"></path>
</svg>)svg"},
        {"fork", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>fork</title>
<path d="M2.016 26.016q0-1.92 1.088-3.456t2.912-2.176v-8.736q-1.792-0.608-2.912-2.144t-1.088-3.488q0-2.496 1.728-4.256t4.256-1.76 4.256 1.76 1.76 4.256q0 1.92-1.12 3.488t-2.88 2.144v4.736q0.992-0.384 1.984-0.384h8q0.832 0 1.408-0.576t0.608-1.408v-0.352q-1.792-0.608-2.912-2.176t-1.088-3.456q0-2.496 1.728-4.256t4.256-1.76 4.256 1.76 1.76 4.256q0 1.92-1.12 3.456t-2.88 2.176v0.352q0 2.496-1.76 4.256t-4.256 1.76h-8q-0.864 0-1.44 0.608 1.536 0.736 2.496 2.176t0.96 3.2q0 2.496-1.76 4.256t-4.256 1.76-4.256-1.76-1.728-4.256zM6.016 26.016q0 0.832 0.576 1.44t1.408 0.576 1.408-0.576 0.608-1.44-0.608-1.408-1.408-0.576-1.408 0.576-0.576 1.408zM6.016 6.016q0 0.832 0.576 1.44t1.408 0.576 1.408-0.576 0.608-1.44-0.608-1.408-1.408-0.576-1.408 0.576-0.576 1.408zM22.016 8.032q0 0.832 0.576 1.408t1.408 0.576 1.408-0.576 0.608-1.408-0.608-1.408-1.408-0.608-1.408 0.608-0.576 1.408z"></path>
</svg>)svg"},
        {"full-cross-circle-alt-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg" id="full-cross-circle-alt" class="icon glyph"><path d="M19.07,4.93A10,10,0,1,0,22,12,10,10,0,0,0,19.07,4.93ZM7.1,18.32A8,8,0,0,1,5.68,16.9L16.9,5.68A8,8,0,0,1,18.32,7.1Z"></path></svg>)svg"},
        {"full-cross-circle-alt-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg" id="full-cross-circle-alt" class="icon glyph"><path d="M19.07,4.93A10,10,0,1,0,22,12,10,10,0,0,0,19.07,4.93ZM7.1,18.32A8,8,0,0,1,5.68,16.9L16.9,5.68A8,8,0,0,1,18.32,7.1Z"></path></svg>)svg"},
        {"full_cross_circle_alt", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg" id="full-cross-circle-alt" class="icon glyph"><path d="M19.07,4.93A10,10,0,1,0,22,12,10,10,0,0,0,19.07,4.93ZM7.1,18.32A8,8,0,0,1,5.68,16.9L16.9,5.68A8,8,0,0,1,18.32,7.1Z"></path></svg>)svg"},
        {"full-screen-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 1920 1920" xmlns="http://www.w3.org/2000/svg">
    <path d="M1146.616-.012V232.38h376.821L232.391 1523.309v-376.705H0V1920h773.629v-232.39H396.69L1687.737 396.68V773.5h232.275V-.011z" fill-rule="evenodd"/>
</svg>)svg"},
        {"full-screen-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 1920 1920" xmlns="http://www.w3.org/2000/svg">
    <path d="M1146.616-.012V232.38h376.821L232.391 1523.309v-376.705H0V1920h773.629v-232.39H396.69L1687.737 396.68V773.5h232.275V-.011z" fill-rule="evenodd"/>
</svg>)svg"},
        {"full_screen", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 1920 1920" xmlns="http://www.w3.org/2000/svg">
    <path d="M1146.616-.012V232.38h376.821L232.391 1523.309v-376.705H0V1920h773.629v-232.39H396.69L1687.737 396.68V773.5h232.275V-.011z" fill-rule="evenodd"/>
</svg>)svg"},
        {"fullscreen-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg width="800px" height="800px" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg"><path fill-rule="evenodd" clip-rule="evenodd" d="M18 4.654v.291a10 10 0 0 0-1.763 1.404l-2.944 2.944a1 1 0 0 0 1.414 1.414l2.933-2.932A9.995 9.995 0 0 0 19.05 6h.296l-.09.39A9.998 9.998 0 0 0 19 8.64v.857a1 1 0 1 0 2 0V4.503a1.5 1.5 0 0 0-1.5-1.5L14.5 3a1 1 0 1 0 0 2h.861a10 10 0 0 0 2.249-.256l.39-.09zM4.95 18a10 10 0 0 1 1.41-1.775l2.933-2.932a1 1 0 0 1 1.414 1.414l-2.944 2.944A9.998 9.998 0 0 1 6 19.055v.291l.39-.09A9.998 9.998 0 0 1 8.64 19H9.5a1 1 0 1 1 0 2l-5-.003a1.5 1.5 0 0 1-1.5-1.5V14.5a1 1 0 1 1 2 0v.861a10 10 0 0 1-.256 2.249l-.09.39h.295z" fill="currentColor"/></svg>)svg"},
        {"fullscreen-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg width="800px" height="800px" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg"><path fill-rule="evenodd" clip-rule="evenodd" d="M18 4.654v.291a10 10 0 0 0-1.763 1.404l-2.944 2.944a1 1 0 0 0 1.414 1.414l2.933-2.932A9.995 9.995 0 0 0 19.05 6h.296l-.09.39A9.998 9.998 0 0 0 19 8.64v.857a1 1 0 1 0 2 0V4.503a1.5 1.5 0 0 0-1.5-1.5L14.5 3a1 1 0 1 0 0 2h.861a10 10 0 0 0 2.249-.256l.39-.09zM4.95 18a10 10 0 0 1 1.41-1.775l2.933-2.932a1 1 0 0 1 1.414 1.414l-2.944 2.944A9.998 9.998 0 0 1 6 19.055v.291l.39-.09A9.998 9.998 0 0 1 8.64 19H9.5a1 1 0 1 1 0 2l-5-.003a1.5 1.5 0 0 1-1.5-1.5V14.5a1 1 0 1 1 2 0v.861a10 10 0 0 1-.256 2.249l-.09.39h.295z" fill="currentColor"/></svg>)svg"},
        {"gdrive-rounded-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>

<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" height="800px" width="800px" version="1.1" id="Layer_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" 
	 viewBox="-143 145 512 512" xml:space="preserve">
<path d="M329,145h-432c-22.1,0-40,17.9-40,40v432c0,22.1,17.9,40,40,40h432c22.1,0,40-17.9,40-40V185C369,162.9,351.1,145,329,145z
	 M23,506.3l-2.1-3.6l-35.2-60.3l-0.7-1.3l0.7-1.2l80.9-140.1l2.1-3.6l2.1,3.6l35.2,60.3l0.7,1.2l-0.7,1.2L25.1,502.6L23,506.3z
	 M238.9,451.6l-34.7,60.6l-0.7,1.2h-1.4H40.4h-4.2l2.1-3.7L73,449.1l0.7-1.2h1.4h161.7h4.2L238.9,451.6z M236.8,433.5l-69.8,0.3
	h-1.4l-0.7-1.2L84,292.5l-2.1-3.7h4.2l69.9-0.3h1.4l0.7,1.2L239,429.8l2.1,3.7H236.8z"/>
</svg>)svg"},
        {"gdrive-rounded-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?>

<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" height="800px" width="800px" version="1.1" id="Layer_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" 
	 viewBox="-143 145 512 512" xml:space="preserve">
<path d="M329,145h-432c-22.1,0-40,17.9-40,40v432c0,22.1,17.9,40,40,40h432c22.1,0,40-17.9,40-40V185C369,162.9,351.1,145,329,145z
	 M23,506.3l-2.1-3.6l-35.2-60.3l-0.7-1.3l0.7-1.2l80.9-140.1l2.1-3.6l2.1,3.6l35.2,60.3l0.7,1.2l-0.7,1.2L25.1,502.6L23,506.3z
	 M238.9,451.6l-34.7,60.6l-0.7,1.2h-1.4H40.4h-4.2l2.1-3.7L73,449.1l0.7-1.2h1.4h161.7h4.2L238.9,451.6z M236.8,433.5l-69.8,0.3
	h-1.4l-0.7-1.2L84,292.5l-2.1-3.7h4.2l69.9-0.3h1.4l0.7,1.2L239,429.8l2.1,3.7H236.8z"/>
</svg>)svg"},
        {"gdrive_rounded", R"svg(<?xml version="1.0" encoding="utf-8"?>

<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" height="800px" width="800px" version="1.1" id="Layer_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" 
	 viewBox="-143 145 512 512" xml:space="preserve">
<path d="M329,145h-432c-22.1,0-40,17.9-40,40v432c0,22.1,17.9,40,40,40h432c22.1,0,40-17.9,40-40V185C369,162.9,351.1,145,329,145z
	 M23,506.3l-2.1-3.6l-35.2-60.3l-0.7-1.3l0.7-1.2l80.9-140.1l2.1-3.6l2.1,3.6l35.2,60.3l0.7,1.2l-0.7,1.2L25.1,502.6L23,506.3z
	 M238.9,451.6l-34.7,60.6l-0.7,1.2h-1.4H40.4h-4.2l2.1-3.7L73,449.1l0.7-1.2h1.4h161.7h4.2L238.9,451.6z M236.8,433.5l-69.8,0.3
	h-1.4l-0.7-1.2L84,292.5l-2.1-3.7h4.2l69.9-0.3h1.4l0.7,1.2L239,429.8l2.1,3.7H236.8z"/>
</svg>)svg"},
        {"gridgapm-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg width="800px" height="800px" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg"><path fill-rule="evenodd" clip-rule="evenodd" d="M5.497 3.006A2.5 2.5 0 0 0 3 5.506V10a1 1 0 0 0 1 1h6a1 1 0 0 0 1-1V4.001a1 1 0 0 0-1.001-1l-4.502.005zM3 14.008a1 1 0 0 1 .999-1l6-.007a1 1 0 0 1 1.001 1V20a1 1 0 0 1-1 1H5.5A2.5 2.5 0 0 1 3 18.5v-4.492zm10 0a1 1 0 0 1 .999-1l6-.007a1 1 0 0 1 1.001 1V18.5a2.5 2.5 0 0 1-2.5 2.5H14a1 1 0 0 1-1-1v-5.992zm0-10a1 1 0 0 1 .999-1l4.498-.005A2.5 2.5 0 0 1 21 5.503V10a1 1 0 0 1-1 1h-6a1 1 0 0 1-1-1V4.008z" fill="currentColor"/></svg>)svg"},
        {"gridgapm-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg width="800px" height="800px" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg"><path fill-rule="evenodd" clip-rule="evenodd" d="M5.497 3.006A2.5 2.5 0 0 0 3 5.506V10a1 1 0 0 0 1 1h6a1 1 0 0 0 1-1V4.001a1 1 0 0 0-1.001-1l-4.502.005zM3 14.008a1 1 0 0 1 .999-1l6-.007a1 1 0 0 1 1.001 1V20a1 1 0 0 1-1 1H5.5A2.5 2.5 0 0 1 3 18.5v-4.492zm10 0a1 1 0 0 1 .999-1l6-.007a1 1 0 0 1 1.001 1V18.5a2.5 2.5 0 0 1-2.5 2.5H14a1 1 0 0 1-1-1v-5.992zm0-10a1 1 0 0 1 .999-1l4.498-.005A2.5 2.5 0 0 1 21 5.503V10a1 1 0 0 1-1 1h-6a1 1 0 0 1-1-1V4.008z" fill="currentColor"/></svg>)svg"},
        {"gridgapm", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg width="800px" height="800px" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg"><path fill-rule="evenodd" clip-rule="evenodd" d="M5.497 3.006A2.5 2.5 0 0 0 3 5.506V10a1 1 0 0 0 1 1h6a1 1 0 0 0 1-1V4.001a1 1 0 0 0-1.001-1l-4.502.005zM3 14.008a1 1 0 0 1 .999-1l6-.007a1 1 0 0 1 1.001 1V20a1 1 0 0 1-1 1H5.5A2.5 2.5 0 0 1 3 18.5v-4.492zm10 0a1 1 0 0 1 .999-1l6-.007a1 1 0 0 1 1.001 1V18.5a2.5 2.5 0 0 1-2.5 2.5H14a1 1 0 0 1-1-1v-5.992zm0-10a1 1 0 0 1 .999-1l4.498-.005A2.5 2.5 0 0 1 21 5.503V10a1 1 0 0 1-1 1h-6a1 1 0 0 1-1-1V4.008z" fill="currentColor"/></svg>)svg"},
        {"heart-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>heart</title>
<path d="M0.256 12.16q0.544 2.080 2.080 3.616l13.664 14.144 13.664-14.144q1.536-1.536 2.080-3.616t0-4.128-2.080-3.584-3.584-2.080-4.16 0-3.584 2.080l-2.336 2.816-2.336-2.816q-1.536-1.536-3.584-2.080t-4.128 0-3.616 2.080-2.080 3.584 0 4.128z"></path>
</svg>)svg"},
        {"heart-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>heart</title>
<path d="M0.256 12.16q0.544 2.080 2.080 3.616l13.664 14.144 13.664-14.144q1.536-1.536 2.080-3.616t0-4.128-2.080-3.584-3.584-2.080-4.16 0-3.584 2.080l-2.336 2.816-2.336-2.816q-1.536-1.536-3.584-2.080t-4.128 0-3.616 2.080-2.080 3.584 0 4.128z"></path>
</svg>)svg"},
        {"heart1-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>heart1</title>
<path d="M24 12.977c-3.866 0-7 3.158-7 7.055 0 2.22 1.020 4.197 2.609 5.491-2.056 1.525-3.609 2.488-3.609 2.488s-14-8.652-14-15.622c0-4.2 2.583-8.399 7.5-8.399 4.5 0 6.5 4.296 6.5 4.296s1.75-4.296 6.5-4.296 7.416 4.115 7.416 8.399c0 0.958-0.272 1.943-0.716 2.932-1.281-1.436-3.134-2.344-5.2-2.344zM24 13.984c3.313 0 6 2.707 6 6.047s-2.687 6.048-6 6.048c-3.314 0-6-2.708-6-6.048s2.686-6.047 6-6.047zM21 21.039h2v2.016h2v-2.016h2v-2.016h-2v-2.016h-2v2.016h-2v2.016z"></path>
</svg>)svg"},
        {"heart1-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>heart1</title>
<path d="M24 12.977c-3.866 0-7 3.158-7 7.055 0 2.22 1.020 4.197 2.609 5.491-2.056 1.525-3.609 2.488-3.609 2.488s-14-8.652-14-15.622c0-4.2 2.583-8.399 7.5-8.399 4.5 0 6.5 4.296 6.5 4.296s1.75-4.296 6.5-4.296 7.416 4.115 7.416 8.399c0 0.958-0.272 1.943-0.716 2.932-1.281-1.436-3.134-2.344-5.2-2.344zM24 13.984c3.313 0 6 2.707 6 6.047s-2.687 6.048-6 6.048c-3.314 0-6-2.708-6-6.048s2.686-6.047 6-6.047zM21 21.039h2v2.016h2v-2.016h2v-2.016h-2v-2.016h-2v2.016h-2v2.016z"></path>
</svg>)svg"},
        {"heart1", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>heart1</title>
<path d="M24 12.977c-3.866 0-7 3.158-7 7.055 0 2.22 1.020 4.197 2.609 5.491-2.056 1.525-3.609 2.488-3.609 2.488s-14-8.652-14-15.622c0-4.2 2.583-8.399 7.5-8.399 4.5 0 6.5 4.296 6.5 4.296s1.75-4.296 6.5-4.296 7.416 4.115 7.416 8.399c0 0.958-0.272 1.943-0.716 2.932-1.281-1.436-3.134-2.344-5.2-2.344zM24 13.984c3.313 0 6 2.707 6 6.047s-2.687 6.048-6 6.048c-3.314 0-6-2.708-6-6.048s2.686-6.047 6-6.047zM21 21.039h2v2.016h2v-2.016h2v-2.016h-2v-2.016h-2v2.016h-2v2.016z"></path>
</svg>)svg"},
        {"heart2-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>heart2</title>
<path d="M24 12.977c-3.866 0-7 3.158-7 7.055 0 2.22 1.020 4.197 2.609 5.491-2.056 1.525-3.609 2.488-3.609 2.488s-14-8.652-14-15.622c0-4.2 2.583-8.399 7.5-8.399 4.5 0 6.5 4.296 6.5 4.296s1.75-4.296 6.5-4.296 7.416 4.115 7.416 8.399c0 0.958-0.272 1.943-0.716 2.932-1.281-1.436-3.134-2.344-5.2-2.344zM24 13.984c3.313 0 6 2.707 6 6.047s-2.687 6.048-6 6.048c-3.314 0-6-2.708-6-6.048s2.686-6.047 6-6.047zM21 21.039h6v-2.016h-6v2.016z"></path>
</svg>)svg"},
        {"heart2-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>heart2</title>
<path d="M24 12.977c-3.866 0-7 3.158-7 7.055 0 2.22 1.020 4.197 2.609 5.491-2.056 1.525-3.609 2.488-3.609 2.488s-14-8.652-14-15.622c0-4.2 2.583-8.399 7.5-8.399 4.5 0 6.5 4.296 6.5 4.296s1.75-4.296 6.5-4.296 7.416 4.115 7.416 8.399c0 0.958-0.272 1.943-0.716 2.932-1.281-1.436-3.134-2.344-5.2-2.344zM24 13.984c3.313 0 6 2.707 6 6.047s-2.687 6.048-6 6.048c-3.314 0-6-2.708-6-6.048s2.686-6.047 6-6.047zM21 21.039h6v-2.016h-6v2.016z"></path>
</svg>)svg"},
        {"heart2", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>heart2</title>
<path d="M24 12.977c-3.866 0-7 3.158-7 7.055 0 2.22 1.020 4.197 2.609 5.491-2.056 1.525-3.609 2.488-3.609 2.488s-14-8.652-14-15.622c0-4.2 2.583-8.399 7.5-8.399 4.5 0 6.5 4.296 6.5 4.296s1.75-4.296 6.5-4.296 7.416 4.115 7.416 8.399c0 0.958-0.272 1.943-0.716 2.932-1.281-1.436-3.134-2.344-5.2-2.344zM24 13.984c3.313 0 6 2.707 6 6.047s-2.687 6.048-6 6.048c-3.314 0-6-2.708-6-6.048s2.686-6.047 6-6.047zM21 21.039h6v-2.016h-6v2.016z"></path>
</svg>)svg"},
        {"image-svgrepo-com (1).svg", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg width="800px" height="800px" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg"><path fill-rule="evenodd" clip-rule="evenodd" d="M6.205 3h11.59c1.114 0 1.519.116 1.926.334.407.218.727.538.945.945.218.407.334.811.334 1.926v7.51l-4.391-4.053a1.5 1.5 0 0 0-2.265.27l-3.13 4.695-2.303-1.48a1.5 1.5 0 0 0-1.96.298L3.005 18.15A12.98 12.98 0 0 1 3 17.795V6.205c0-1.115.116-1.519.334-1.926.218-.407.538-.727.945-.945C4.686 3.116 5.09 3 6.205 3zm9.477 8.53L21 16.437v1.357c0 1.114-.116 1.519-.334 1.926a2.272 2.272 0 0 1-.945.945c-.407.218-.811.334-1.926.334H6.205c-1.115 0-1.519-.116-1.926-.334a2.305 2.305 0 0 1-.485-.345L8.2 15.067l2.346 1.508a1.5 1.5 0 0 0 2.059-.43l3.077-4.616zM7.988 6C6.878 6 6 6.832 6 7.988 6 9.145 6.879 10 7.988 10 9.121 10 10 9.145 10 7.988 10 6.832 9.121 6 7.988 6z" fill="currentColor"/></svg>)svg"},
        {"image-svgrepo-com (1)", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg width="800px" height="800px" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg"><path fill-rule="evenodd" clip-rule="evenodd" d="M6.205 3h11.59c1.114 0 1.519.116 1.926.334.407.218.727.538.945.945.218.407.334.811.334 1.926v7.51l-4.391-4.053a1.5 1.5 0 0 0-2.265.27l-3.13 4.695-2.303-1.48a1.5 1.5 0 0 0-1.96.298L3.005 18.15A12.98 12.98 0 0 1 3 17.795V6.205c0-1.115.116-1.519.334-1.926.218-.407.538-.727.945-.945C4.686 3.116 5.09 3 6.205 3zm9.477 8.53L21 16.437v1.357c0 1.114-.116 1.519-.334 1.926a2.272 2.272 0 0 1-.945.945c-.407.218-.811.334-1.926.334H6.205c-1.115 0-1.519-.116-1.926-.334a2.305 2.305 0 0 1-.485-.345L8.2 15.067l2.346 1.508a1.5 1.5 0 0 0 2.059-.43l3.077-4.616zM7.988 6C6.878 6 6 6.832 6 7.988 6 9.145 6.879 10 7.988 10 9.121 10 10 9.145 10 7.988 10 6.832 9.121 6 7.988 6z" fill="currentColor"/></svg>)svg"},
        {"image-svgrepo-com (2).svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>image</title>
<path d="M28 29v-1h-1v-1h-1v-13h1v-1h1v-1h1v17h-1zM27 13h-1v1h-20v-1h-1v-1h-1v-1h24v1h-1v1zM5 13v1h1v13h-1v1h-1v1h-1v-17h1v1h1zM5 28h1v-1h20v1h1v1h1v1h-24v-1h1v-1zM12.856 22.982l2.286-3.697 2.286 2.304 3.143-4.731 3.286 8.142h-14.572l1.857-3.715 1.714 1.697zM11.491 19.009c-0.829 0-1.5-0.672-1.5-1.5s0.671-1.5 1.5-1.5c0.828 0 1.5 0.672 1.5 1.5s-0.672 1.5-1.5 1.5zM25 11l-7.322-5.45c-0.344 0.277-0.775 0.45-1.25 0.45-0.661 0-1.244-0.325-1.607-0.821l-7.821 5.821h-1l8.493-6.518c-0.038-0.155-0.064-0.315-0.064-0.482 0-1.104 0.895-2 1.999-2 1.105 0 2 0.896 2 2 0 0.359-0.103 0.692-0.269 0.984l7.841 6.016h-1z"></path>
</svg>)svg"},
        {"image-svgrepo-com (2)", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>image</title>
<path d="M28 29v-1h-1v-1h-1v-13h1v-1h1v-1h1v17h-1zM27 13h-1v1h-20v-1h-1v-1h-1v-1h24v1h-1v1zM5 13v1h1v13h-1v1h-1v1h-1v-17h1v1h1zM5 28h1v-1h20v1h1v1h1v1h-24v-1h1v-1zM12.856 22.982l2.286-3.697 2.286 2.304 3.143-4.731 3.286 8.142h-14.572l1.857-3.715 1.714 1.697zM11.491 19.009c-0.829 0-1.5-0.672-1.5-1.5s0.671-1.5 1.5-1.5c0.828 0 1.5 0.672 1.5 1.5s-0.672 1.5-1.5 1.5zM25 11l-7.322-5.45c-0.344 0.277-0.775 0.45-1.25 0.45-0.661 0-1.244-0.325-1.607-0.821l-7.821 5.821h-1l8.493-6.518c-0.038-0.155-0.064-0.315-0.064-0.482 0-1.104 0.895-2 1.999-2 1.105 0 2 0.896 2 2 0 0.359-0.103 0.692-0.269 0.984l7.841 6.016h-1z"></path>
</svg>)svg"},
        {"image-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>image</title>
<path d="M0 26.016q0 2.496 1.76 4.224t4.256 1.76h20q2.464 0 4.224-1.76t1.76-4.224v-20q0-2.496-1.76-4.256t-4.224-1.76h-20q-2.496 0-4.256 1.76t-1.76 4.256v20zM4 26.016v-20q0-0.832 0.576-1.408t1.44-0.608h20q0.8 0 1.408 0.608t0.576 1.408v20q0 0.832-0.576 1.408t-1.408 0.576h-20q-0.832 0-1.44-0.576t-0.576-1.408zM6.016 24q0 0.832 0.576 1.44t1.408 0.576h16q0.832 0 1.408-0.576t0.608-1.44v-0.928q-0.224-0.448-1.12-2.688t-1.6-3.584-1.28-2.112q-0.544-0.576-1.12-0.608t-1.152 0.384-1.152 1.12-1.184 1.568-1.152 1.696-1.152 1.6-1.088 1.184-1.088 0.448q-0.576 0-1.664-1.44-0.16-0.192-0.48-0.608-1.12-1.504-1.6-1.824-0.768-0.512-1.184 0.352-0.224 0.512-0.928 2.24t-1.056 2.56v0.64zM6.016 9.024q0 1.248 0.864 2.112t2.112 0.864 2.144-0.864 0.864-2.112-0.864-2.144-2.144-0.864-2.112 0.864-0.864 2.144z"></path>
</svg>)svg"},
        {"image-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>image</title>
<path d="M0 26.016q0 2.496 1.76 4.224t4.256 1.76h20q2.464 0 4.224-1.76t1.76-4.224v-20q0-2.496-1.76-4.256t-4.224-1.76h-20q-2.496 0-4.256 1.76t-1.76 4.256v20zM4 26.016v-20q0-0.832 0.576-1.408t1.44-0.608h20q0.8 0 1.408 0.608t0.576 1.408v20q0 0.832-0.576 1.408t-1.408 0.576h-20q-0.832 0-1.44-0.576t-0.576-1.408zM6.016 24q0 0.832 0.576 1.44t1.408 0.576h16q0.832 0 1.408-0.576t0.608-1.44v-0.928q-0.224-0.448-1.12-2.688t-1.6-3.584-1.28-2.112q-0.544-0.576-1.12-0.608t-1.152 0.384-1.152 1.12-1.184 1.568-1.152 1.696-1.152 1.6-1.088 1.184-1.088 0.448q-0.576 0-1.664-1.44-0.16-0.192-0.48-0.608-1.12-1.504-1.6-1.824-0.768-0.512-1.184 0.352-0.224 0.512-0.928 2.24t-1.056 2.56v0.64zM6.016 9.024q0 1.248 0.864 2.112t2.112 0.864 2.144-0.864 0.864-2.112-0.864-2.144-2.144-0.864-2.112 0.864-0.864 2.144z"></path>
</svg>)svg"},
        {"inbox-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 1920 1920" xmlns="http://www.w3.org/2000/svg">
    <path d="M1750.588 1750.118H169.412c-31.172 0-56.47-25.412-56.47-56.47V1398.08l210.183 126.155h1273.75l210.184-126.155v295.567c0 31.059-25.299 56.47-56.47 56.47ZM451.765 1411.294V168.941h1016.47v1242.353H451.765Zm1298.823-677.647h-169.412V56H338.824v677.647H169.412C76.009 733.647 0 809.657 0 903.06v790.588c0 93.402 76.01 169.412 169.412 169.412h1581.176c93.403 0 169.412-76.01 169.412-169.412V903.06c0-93.403-76.01-169.412-169.412-169.412ZM621.176 488.904h564.706V375.962H621.176v112.942Zm0 677.647h564.706v-112.942H621.176v112.942Zm0-338.824h677.648V714.786H621.176v112.941Z" fill-rule="evenodd"/>
</svg>)svg"},
        {"inbox-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 1920 1920" xmlns="http://www.w3.org/2000/svg">
    <path d="M1750.588 1750.118H169.412c-31.172 0-56.47-25.412-56.47-56.47V1398.08l210.183 126.155h1273.75l210.184-126.155v295.567c0 31.059-25.299 56.47-56.47 56.47ZM451.765 1411.294V168.941h1016.47v1242.353H451.765Zm1298.823-677.647h-169.412V56H338.824v677.647H169.412C76.009 733.647 0 809.657 0 903.06v790.588c0 93.402 76.01 169.412 169.412 169.412h1581.176c93.403 0 169.412-76.01 169.412-169.412V903.06c0-93.403-76.01-169.412-169.412-169.412ZM621.176 488.904h564.706V375.962H621.176v112.942Zm0 677.647h564.706v-112.942H621.176v112.942Zm0-338.824h677.648V714.786H621.176v112.941Z" fill-rule="evenodd"/>
</svg>)svg"},
        {"infinity-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>infinity</title>
<path d="M0 16q0 2.496 1.216 4.608t3.328 3.328 4.608 1.216q1.504 0 3.072-0.576-1.472-1.824-2.272-4.064-0.544 0.064-0.8 0.064-1.888 0-3.232-1.344t-1.344-3.232 1.344-3.232 3.232-1.344 3.232 1.344 1.344 3.232q0 2.496 1.216 4.608t3.328 3.328 4.576 1.216 4.608-1.216 3.328-3.328 1.216-4.608-1.216-4.576-3.328-3.328-4.608-1.216q-1.504 0-3.072 0.544 1.472 1.824 2.272 4.096 0.544-0.096 0.8-0.096 1.888 0 3.232 1.344t1.344 3.232q0 1.92-1.344 3.264t-3.232 1.312q-1.888 0-3.232-1.312t-1.344-3.264q0-2.464-1.216-4.576t-3.328-3.328-4.576-1.216-4.608 1.216-3.328 3.328-1.216 4.576z"></path>
</svg>)svg"},
        {"infinity-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>infinity</title>
<path d="M0 16q0 2.496 1.216 4.608t3.328 3.328 4.608 1.216q1.504 0 3.072-0.576-1.472-1.824-2.272-4.064-0.544 0.064-0.8 0.064-1.888 0-3.232-1.344t-1.344-3.232 1.344-3.232 3.232-1.344 3.232 1.344 1.344 3.232q0 2.496 1.216 4.608t3.328 3.328 4.576 1.216 4.608-1.216 3.328-3.328 1.216-4.608-1.216-4.576-3.328-3.328-4.608-1.216q-1.504 0-3.072 0.544 1.472 1.824 2.272 4.096 0.544-0.096 0.8-0.096 1.888 0 3.232 1.344t1.344 3.232q0 1.92-1.344 3.264t-3.232 1.312q-1.888 0-3.232-1.312t-1.344-3.264q0-2.464-1.216-4.576t-3.328-3.328-4.576-1.216-4.608 1.216-3.328 3.328-1.216 4.576z"></path>
</svg>)svg"},
        {"infinity", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>infinity</title>
<path d="M0 16q0 2.496 1.216 4.608t3.328 3.328 4.608 1.216q1.504 0 3.072-0.576-1.472-1.824-2.272-4.064-0.544 0.064-0.8 0.064-1.888 0-3.232-1.344t-1.344-3.232 1.344-3.232 3.232-1.344 3.232 1.344 1.344 3.232q0 2.496 1.216 4.608t3.328 3.328 4.576 1.216 4.608-1.216 3.328-3.328 1.216-4.608-1.216-4.576-3.328-3.328-4.608-1.216q-1.504 0-3.072 0.544 1.472 1.824 2.272 4.096 0.544-0.096 0.8-0.096 1.888 0 3.232 1.344t1.344 3.232q0 1.92-1.344 3.264t-3.232 1.312q-1.888 0-3.232-1.312t-1.344-3.264q0-2.464-1.216-4.576t-3.328-3.328-4.576-1.216-4.608 1.216-3.328 3.328-1.216 4.576z"></path>
</svg>)svg"},
        {"injection-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>injection</title>
<path d="M24.16 9.207c0.378 0.377 0.378 0.989 0 1.367s-0.685 0.684-0.685 0.684l0.685 0.684c0.755 0.755 0.755 1.979 0 2.735l-10.255 10.253c-0.756 0.756-1.98 0.756-2.735 0l-0.684-0.684-2.734 2.736 1.367 1.367c0.378 0.377 0.378 0.988 0 1.367-0.378 0.377-0.99 0.377-1.367 0l-5.47-5.471c-0.377-0.377-0.377-0.988 0-1.367 0.378-0.377 0.99-0.377 1.367 0l1.367 1.367 2.735-2.734-0.684-0.684c-0.756-0.754-0.756-1.979 0-2.734l10.256-10.254c0.756-0.756 1.979-0.756 2.734 0l0.684 0.684c0 0 0.307-0.307 0.684-0.684 0.378-0.378 0.99-0.378 1.367 0l0.342 0.342 5.47-5.47v1.367l-4.786 4.786 0.342 0.343zM19.374 8.523c-0.377-0.378-0.989-0.378-1.367 0l-2.051 2.051 1.367 1.367-0.684 0.684-1.367-1.367-2.051 2.051 1.367 1.368-0.684 0.684-1.368-1.368-0.684 0.684 2.735 2.734-0.684 0.684-2.735-2.734-2.049 2.049 2.734 2.734-0.684 0.684-2.734-2.734-0.684 0.684c-0.378 0.377-0.378 0.99 0 1.367l4.102 4.102c0.378 0.379 0.99 0.379 1.368 0l10.254-10.254c0.379-0.377 0.379-0.989 0-1.367l-4.101-4.103zM16.64 9.89l0.684-0.684 2.734 2.734-0.684 0.684-2.734-2.734zM16.64 15.359l-2.734-2.735 0.684-0.684 2.734 2.735-0.684 0.684zM9.803 16.727l0.684-0.684 1.367 1.367-0.684 0.684-1.367-1.367zM29.942 9.386c0 0.725-0.588 1.312-1.312 1.312-0.726 0-1.313-0.588-1.313-1.312 0-0.726 1.313-2.708 1.313-2.708s1.312 1.983 1.312 2.708z"></path>
</svg>)svg"},
        {"injection-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>injection</title>
<path d="M24.16 9.207c0.378 0.377 0.378 0.989 0 1.367s-0.685 0.684-0.685 0.684l0.685 0.684c0.755 0.755 0.755 1.979 0 2.735l-10.255 10.253c-0.756 0.756-1.98 0.756-2.735 0l-0.684-0.684-2.734 2.736 1.367 1.367c0.378 0.377 0.378 0.988 0 1.367-0.378 0.377-0.99 0.377-1.367 0l-5.47-5.471c-0.377-0.377-0.377-0.988 0-1.367 0.378-0.377 0.99-0.377 1.367 0l1.367 1.367 2.735-2.734-0.684-0.684c-0.756-0.754-0.756-1.979 0-2.734l10.256-10.254c0.756-0.756 1.979-0.756 2.734 0l0.684 0.684c0 0 0.307-0.307 0.684-0.684 0.378-0.378 0.99-0.378 1.367 0l0.342 0.342 5.47-5.47v1.367l-4.786 4.786 0.342 0.343zM19.374 8.523c-0.377-0.378-0.989-0.378-1.367 0l-2.051 2.051 1.367 1.367-0.684 0.684-1.367-1.367-2.051 2.051 1.367 1.368-0.684 0.684-1.368-1.368-0.684 0.684 2.735 2.734-0.684 0.684-2.735-2.734-2.049 2.049 2.734 2.734-0.684 0.684-2.734-2.734-0.684 0.684c-0.378 0.377-0.378 0.99 0 1.367l4.102 4.102c0.378 0.379 0.99 0.379 1.368 0l10.254-10.254c0.379-0.377 0.379-0.989 0-1.367l-4.101-4.103zM16.64 9.89l0.684-0.684 2.734 2.734-0.684 0.684-2.734-2.734zM16.64 15.359l-2.734-2.735 0.684-0.684 2.734 2.735-0.684 0.684zM9.803 16.727l0.684-0.684 1.367 1.367-0.684 0.684-1.367-1.367zM29.942 9.386c0 0.725-0.588 1.312-1.312 1.312-0.726 0-1.313-0.588-1.313-1.312 0-0.726 1.313-2.708 1.313-2.708s1.312 1.983 1.312 2.708z"></path>
</svg>)svg"},
        {"injection", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>injection</title>
<path d="M24.16 9.207c0.378 0.377 0.378 0.989 0 1.367s-0.685 0.684-0.685 0.684l0.685 0.684c0.755 0.755 0.755 1.979 0 2.735l-10.255 10.253c-0.756 0.756-1.98 0.756-2.735 0l-0.684-0.684-2.734 2.736 1.367 1.367c0.378 0.377 0.378 0.988 0 1.367-0.378 0.377-0.99 0.377-1.367 0l-5.47-5.471c-0.377-0.377-0.377-0.988 0-1.367 0.378-0.377 0.99-0.377 1.367 0l1.367 1.367 2.735-2.734-0.684-0.684c-0.756-0.754-0.756-1.979 0-2.734l10.256-10.254c0.756-0.756 1.979-0.756 2.734 0l0.684 0.684c0 0 0.307-0.307 0.684-0.684 0.378-0.378 0.99-0.378 1.367 0l0.342 0.342 5.47-5.47v1.367l-4.786 4.786 0.342 0.343zM19.374 8.523c-0.377-0.378-0.989-0.378-1.367 0l-2.051 2.051 1.367 1.367-0.684 0.684-1.367-1.367-2.051 2.051 1.367 1.368-0.684 0.684-1.368-1.368-0.684 0.684 2.735 2.734-0.684 0.684-2.735-2.734-2.049 2.049 2.734 2.734-0.684 0.684-2.734-2.734-0.684 0.684c-0.378 0.377-0.378 0.99 0 1.367l4.102 4.102c0.378 0.379 0.99 0.379 1.368 0l10.254-10.254c0.379-0.377 0.379-0.989 0-1.367l-4.101-4.103zM16.64 9.89l0.684-0.684 2.734 2.734-0.684 0.684-2.734-2.734zM16.64 15.359l-2.734-2.735 0.684-0.684 2.734 2.735-0.684 0.684zM9.803 16.727l0.684-0.684 1.367 1.367-0.684 0.684-1.367-1.367zM29.942 9.386c0 0.725-0.588 1.312-1.312 1.312-0.726 0-1.313-0.588-1.313-1.312 0-0.726 1.313-2.708 1.313-2.708s1.312 1.983 1.312 2.708z"></path>
</svg>)svg"},
        {"intersect-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg"><path d="M15,4V6a1,1,0,0,1-1,1H7.5a.5.5,0,0,0-.5.5V14a1,1,0,0,1-1,1H4a1,1,0,0,1-1-1V4A1,1,0,0,1,4,3H14A1,1,0,0,1,15,4Zm6,16V10a1,1,0,0,0-1-1H18a1,1,0,0,0-1,1v6.5a.5.5,0,0,1-.5.5H10a1,1,0,0,0-1,1v2a1,1,0,0,0,1,1H20A1,1,0,0,0,21,20ZM10,15h4a1,1,0,0,0,1-1V10a1,1,0,0,0-1-1H10a1,1,0,0,0-1,1v4A1,1,0,0,0,10,15Z"/></svg>)svg"},
        {"intersect-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg"><path d="M15,4V6a1,1,0,0,1-1,1H7.5a.5.5,0,0,0-.5.5V14a1,1,0,0,1-1,1H4a1,1,0,0,1-1-1V4A1,1,0,0,1,4,3H14A1,1,0,0,1,15,4Zm6,16V10a1,1,0,0,0-1-1H18a1,1,0,0,0-1,1v6.5a.5.5,0,0,1-.5.5H10a1,1,0,0,0-1,1v2a1,1,0,0,0,1,1H20A1,1,0,0,0,21,20ZM10,15h4a1,1,0,0,0,1-1V10a1,1,0,0,0-1-1H10a1,1,0,0,0-1,1v4A1,1,0,0,0,10,15Z"/></svg>)svg"},
        {"intersect", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg"><path d="M15,4V6a1,1,0,0,1-1,1H7.5a.5.5,0,0,0-.5.5V14a1,1,0,0,1-1,1H4a1,1,0,0,1-1-1V4A1,1,0,0,1,4,3H14A1,1,0,0,1,15,4Zm6,16V10a1,1,0,0,0-1-1H18a1,1,0,0,0-1,1v6.5a.5.5,0,0,1-.5.5H10a1,1,0,0,0-1,1v2a1,1,0,0,0,1,1H20A1,1,0,0,0,21,20ZM10,15h4a1,1,0,0,0,1-1V10a1,1,0,0,0-1-1H10a1,1,0,0,0-1,1v4A1,1,0,0,0,10,15Z"/></svg>)svg"},
        {"label-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>label</title>
<path d="M0 7.008v-3.008q0-1.632 1.184-2.816t2.816-1.184h4q1.664 0 2.816 1.184t1.184 2.816h4q2.496 0 4.256 1.76l9.984 10.016q1.76 1.728 1.76 4.224t-1.76 4.256l-5.984 6.016q-1.76 1.728-4.224 1.728t-4.256-1.728l-10.016-10.016q-1.76-1.76-1.76-4.256v-12h6.016q0-0.832-0.608-1.408t-1.408-0.576h-4q-0.832 0-1.408 0.576t-0.576 1.408v3.008q0 0.608-0.512 0.864t-0.992 0-0.512-0.864zM8 16q0 0.832 0.608 1.408l9.984 10.016q0.608 0.576 1.44 0.576t1.376-0.576l6.016-6.016q0.576-0.576 0.576-1.408t-0.576-1.408l-10.016-10.016q-0.576-0.576-1.408-0.576h-1.024q1.024 1.376 1.024 3.008 0 1.12-0.384 2.048t-0.992 1.536-1.472 0.992-1.728 0.416-1.76-0.192-1.664-0.832v1.024zM8 11.008q0 0.8 0.32 1.44t0.864 0.928 1.184 0.48 1.28 0 1.152-0.48 0.864-0.928 0.352-1.44q0-0.96-0.576-1.728t-1.44-1.056v2.784q0 0.608-0.512 0.864t-0.992 0-0.48-0.864v-2.784q-0.896 0.288-1.44 1.056t-0.576 1.728z"></path>
</svg>)svg"},
        {"label-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>label</title>
<path d="M0 7.008v-3.008q0-1.632 1.184-2.816t2.816-1.184h4q1.664 0 2.816 1.184t1.184 2.816h4q2.496 0 4.256 1.76l9.984 10.016q1.76 1.728 1.76 4.224t-1.76 4.256l-5.984 6.016q-1.76 1.728-4.224 1.728t-4.256-1.728l-10.016-10.016q-1.76-1.76-1.76-4.256v-12h6.016q0-0.832-0.608-1.408t-1.408-0.576h-4q-0.832 0-1.408 0.576t-0.576 1.408v3.008q0 0.608-0.512 0.864t-0.992 0-0.512-0.864zM8 16q0 0.832 0.608 1.408l9.984 10.016q0.608 0.576 1.44 0.576t1.376-0.576l6.016-6.016q0.576-0.576 0.576-1.408t-0.576-1.408l-10.016-10.016q-0.576-0.576-1.408-0.576h-1.024q1.024 1.376 1.024 3.008 0 1.12-0.384 2.048t-0.992 1.536-1.472 0.992-1.728 0.416-1.76-0.192-1.664-0.832v1.024zM8 11.008q0 0.8 0.32 1.44t0.864 0.928 1.184 0.48 1.28 0 1.152-0.48 0.864-0.928 0.352-1.44q0-0.96-0.576-1.728t-1.44-1.056v2.784q0 0.608-0.512 0.864t-0.992 0-0.48-0.864v-2.784q-0.896 0.288-1.44 1.056t-0.576 1.728z"></path>
</svg>)svg"},
        {"label", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>label</title>
<path d="M0 7.008v-3.008q0-1.632 1.184-2.816t2.816-1.184h4q1.664 0 2.816 1.184t1.184 2.816h4q2.496 0 4.256 1.76l9.984 10.016q1.76 1.728 1.76 4.224t-1.76 4.256l-5.984 6.016q-1.76 1.728-4.224 1.728t-4.256-1.728l-10.016-10.016q-1.76-1.76-1.76-4.256v-12h6.016q0-0.832-0.608-1.408t-1.408-0.576h-4q-0.832 0-1.408 0.576t-0.576 1.408v3.008q0 0.608-0.512 0.864t-0.992 0-0.512-0.864zM8 16q0 0.832 0.608 1.408l9.984 10.016q0.608 0.576 1.44 0.576t1.376-0.576l6.016-6.016q0.576-0.576 0.576-1.408t-0.576-1.408l-10.016-10.016q-0.576-0.576-1.408-0.576h-1.024q1.024 1.376 1.024 3.008 0 1.12-0.384 2.048t-0.992 1.536-1.472 0.992-1.728 0.416-1.76-0.192-1.664-0.832v1.024zM8 11.008q0 0.8 0.32 1.44t0.864 0.928 1.184 0.48 1.28 0 1.152-0.48 0.864-0.928 0.352-1.44q0-0.96-0.576-1.728t-1.44-1.056v2.784q0 0.608-0.512 0.864t-0.992 0-0.48-0.864v-2.784q-0.896 0.288-1.44 1.056t-0.576 1.728z"></path>
</svg>)svg"},
        {"lamp1-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>lamp1</title>
<path d="M17.57 18.118c-1.598-1.306-2.618-3.292-2.618-5.517 0-1.118 0.28-2.162 0.739-3.104l-1.098-1.097-6.67 6.671c1.24 0.976 1.115 1.93-0.336 2.86l8.911 8.91h4.367c0.562 0 1.018 0.456 1.018 1.018v1.018h-4.768l-0.011 0.011-0.011-0.011h-7.418v-1.017c0-0.562 0.456-1.018 1.018-1.018h4.367l-8.406-8.405c-1.661 0.712-2.481 0.024-2.446-2.090 0.038-2.249 0.991-2.849 2.839-1.837l6.829-6.829-0.676-0.675c-0.397-0.396-0.397-1.041 0-1.438l2.158-2.158c0.397-0.397 1.042-0.397 1.439 0l2.6 2.602c0.828-0.337 1.729-0.53 2.678-0.53 2.342 0 4.422 1.005 5.719 2.752 0.044 0.011-10.187 9.9-10.224 9.884zM25.443 11.754c0 0 1.717 2.161 0 3.878s-3.857-0.021-3.857-0.021l3.857-3.857z"></path>
</svg>)svg"},
        {"lamp1-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>lamp1</title>
<path d="M17.57 18.118c-1.598-1.306-2.618-3.292-2.618-5.517 0-1.118 0.28-2.162 0.739-3.104l-1.098-1.097-6.67 6.671c1.24 0.976 1.115 1.93-0.336 2.86l8.911 8.91h4.367c0.562 0 1.018 0.456 1.018 1.018v1.018h-4.768l-0.011 0.011-0.011-0.011h-7.418v-1.017c0-0.562 0.456-1.018 1.018-1.018h4.367l-8.406-8.405c-1.661 0.712-2.481 0.024-2.446-2.090 0.038-2.249 0.991-2.849 2.839-1.837l6.829-6.829-0.676-0.675c-0.397-0.396-0.397-1.041 0-1.438l2.158-2.158c0.397-0.397 1.042-0.397 1.439 0l2.6 2.602c0.828-0.337 1.729-0.53 2.678-0.53 2.342 0 4.422 1.005 5.719 2.752 0.044 0.011-10.187 9.9-10.224 9.884zM25.443 11.754c0 0 1.717 2.161 0 3.878s-3.857-0.021-3.857-0.021l3.857-3.857z"></path>
</svg>)svg"},
        {"lamp1", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>lamp1</title>
<path d="M17.57 18.118c-1.598-1.306-2.618-3.292-2.618-5.517 0-1.118 0.28-2.162 0.739-3.104l-1.098-1.097-6.67 6.671c1.24 0.976 1.115 1.93-0.336 2.86l8.911 8.91h4.367c0.562 0 1.018 0.456 1.018 1.018v1.018h-4.768l-0.011 0.011-0.011-0.011h-7.418v-1.017c0-0.562 0.456-1.018 1.018-1.018h4.367l-8.406-8.405c-1.661 0.712-2.481 0.024-2.446-2.090 0.038-2.249 0.991-2.849 2.839-1.837l6.829-6.829-0.676-0.675c-0.397-0.396-0.397-1.041 0-1.438l2.158-2.158c0.397-0.397 1.042-0.397 1.439 0l2.6 2.602c0.828-0.337 1.729-0.53 2.678-0.53 2.342 0 4.422 1.005 5.719 2.752 0.044 0.011-10.187 9.9-10.224 9.884zM25.443 11.754c0 0 1.717 2.161 0 3.878s-3.857-0.021-3.857-0.021l3.857-3.857z"></path>
</svg>)svg"},
        {"launch-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 1920 1920" xmlns="http://www.w3.org/2000/svg">
    <path d="M508.235 0H56.471C25.299 0 0 25.299 0 56.47v451.765c0 31.172 25.299 56.47 56.47 56.47h451.765c31.172 0 56.47-25.298 56.47-56.47V56.471C564.706 25.299 539.408 0 508.236 0zm677.647 0H734.118c-31.172 0-56.47 25.299-56.47 56.47v451.765c0 31.172 25.298 56.47 56.47 56.47h451.764c31.172 0 56.47-25.298 56.47-56.47V56.471c0-31.172-25.298-56.471-56.47-56.471zm677.647 0h-451.764c-31.172 0-56.47 25.299-56.47 56.47v451.765c0 31.172 25.298 56.47 56.47 56.47h451.764c31.172 0 56.471-25.298 56.471-56.47V56.471C1920 25.299 1894.701 0 1863.53 0zM508.235 677.647H56.471C25.299 677.647 0 702.946 0 734.117v451.765c0 31.172 25.299 56.47 56.47 56.47h451.765c31.172 0 56.47-25.298 56.47-56.47V734.118c0-31.172-25.298-56.47-56.47-56.47zm677.647 0H734.118c-31.172 0-56.47 25.299-56.47 56.47v451.765c0 31.172 25.298 56.47 56.47 56.47h451.764c31.172 0 56.47-25.298 56.47-56.47V734.118c0-31.172-25.298-56.47-56.47-56.47zm677.647 0h-451.764c-31.172 0-56.47 25.299-56.47 56.47v451.765c0 31.172 25.298 56.47 56.47 56.47h451.764c31.172 0 56.471-25.298 56.471-56.47V734.118c0-31.172-25.299-56.47-56.47-56.47zM508.235 1355.294H56.471c-31.172 0-56.471 25.299-56.471 56.47v451.765C0 1894.701 25.299 1920 56.47 1920h451.765c31.172 0 56.47-25.299 56.47-56.47v-451.765c0-31.172-25.298-56.47-56.47-56.47zm677.647 0H734.118c-31.172 0-56.47 25.299-56.47 56.47v451.765c0 31.172 25.298 56.471 56.47 56.471h451.764c31.172 0 56.47-25.299 56.47-56.47v-451.765c0-31.172-25.298-56.47-56.47-56.47zm677.647 0h-451.764c-31.172 0-56.47 25.299-56.47 56.47v451.765c0 31.172 25.298 56.471 56.47 56.471h451.764c31.172 0 56.471-25.299 56.471-56.47v-451.765c0-31.172-25.299-56.47-56.47-56.47z" fill-rule="evenodd"/>
</svg>)svg"},
        {"launch-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 1920 1920" xmlns="http://www.w3.org/2000/svg">
    <path d="M508.235 0H56.471C25.299 0 0 25.299 0 56.47v451.765c0 31.172 25.299 56.47 56.47 56.47h451.765c31.172 0 56.47-25.298 56.47-56.47V56.471C564.706 25.299 539.408 0 508.236 0zm677.647 0H734.118c-31.172 0-56.47 25.299-56.47 56.47v451.765c0 31.172 25.298 56.47 56.47 56.47h451.764c31.172 0 56.47-25.298 56.47-56.47V56.471c0-31.172-25.298-56.471-56.47-56.471zm677.647 0h-451.764c-31.172 0-56.47 25.299-56.47 56.47v451.765c0 31.172 25.298 56.47 56.47 56.47h451.764c31.172 0 56.471-25.298 56.471-56.47V56.471C1920 25.299 1894.701 0 1863.53 0zM508.235 677.647H56.471C25.299 677.647 0 702.946 0 734.117v451.765c0 31.172 25.299 56.47 56.47 56.47h451.765c31.172 0 56.47-25.298 56.47-56.47V734.118c0-31.172-25.298-56.47-56.47-56.47zm677.647 0H734.118c-31.172 0-56.47 25.299-56.47 56.47v451.765c0 31.172 25.298 56.47 56.47 56.47h451.764c31.172 0 56.47-25.298 56.47-56.47V734.118c0-31.172-25.298-56.47-56.47-56.47zm677.647 0h-451.764c-31.172 0-56.47 25.299-56.47 56.47v451.765c0 31.172 25.298 56.47 56.47 56.47h451.764c31.172 0 56.471-25.298 56.471-56.47V734.118c0-31.172-25.299-56.47-56.47-56.47zM508.235 1355.294H56.471c-31.172 0-56.471 25.299-56.471 56.47v451.765C0 1894.701 25.299 1920 56.47 1920h451.765c31.172 0 56.47-25.299 56.47-56.47v-451.765c0-31.172-25.298-56.47-56.47-56.47zm677.647 0H734.118c-31.172 0-56.47 25.299-56.47 56.47v451.765c0 31.172 25.298 56.471 56.47 56.471h451.764c31.172 0 56.47-25.299 56.47-56.47v-451.765c0-31.172-25.298-56.47-56.47-56.47zm677.647 0h-451.764c-31.172 0-56.47 25.299-56.47 56.47v451.765c0 31.172 25.298 56.471 56.47 56.471h451.764c31.172 0 56.471-25.299 56.471-56.47v-451.765c0-31.172-25.299-56.47-56.47-56.47z" fill-rule="evenodd"/>
</svg>)svg"},
        {"layout-2-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg"><path d="M11,11H3V4A1,1,0,0,1,4,3h7ZM21,4a1,1,0,0,0-1-1H13v8h8ZM4,21h7V13H3v7A1,1,0,0,0,4,21Zm17-1V13H13v8h7A1,1,0,0,0,21,20Z"/></svg>)svg"},
        {"layout-2-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg"><path d="M11,11H3V4A1,1,0,0,1,4,3h7ZM21,4a1,1,0,0,0-1-1H13v8h8ZM4,21h7V13H3v7A1,1,0,0,0,4,21Zm17-1V13H13v8h7A1,1,0,0,0,21,20Z"/></svg>)svg"},
        {"layout_2", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg"><path d="M11,11H3V4A1,1,0,0,1,4,3h7ZM21,4a1,1,0,0,0-1-1H13v8h8ZM4,21h7V13H3v7A1,1,0,0,0,4,21Zm17-1V13H13v8h7A1,1,0,0,0,21,20Z"/></svg>)svg"},
        {"layout-3-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg"><path d="M21,14.5H13v-5h8ZM4,21h7V3H4A1,1,0,0,0,3,4V20A1,1,0,0,0,4,21ZM21,4a1,1,0,0,0-1-1H13V8h8Zm0,16V16H13v5h7A1,1,0,0,0,21,20Z"/></svg>)svg"},
        {"layout-3-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg"><path d="M21,14.5H13v-5h8ZM4,21h7V3H4A1,1,0,0,0,3,4V20A1,1,0,0,0,4,21ZM21,4a1,1,0,0,0-1-1H13V8h8Zm0,16V16H13v5h7A1,1,0,0,0,21,20Z"/></svg>)svg"},
        {"layout_3", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg"><path d="M21,14.5H13v-5h8ZM4,21h7V3H4A1,1,0,0,0,3,4V20A1,1,0,0,0,4,21ZM21,4a1,1,0,0,0-1-1H13V8h8Zm0,16V16H13v5h7A1,1,0,0,0,21,20Z"/></svg>)svg"},
        {"layout-4-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg"><path d="M3,20V4A1,1,0,0,1,4,3h7V21H4A1,1,0,0,1,3,20Zm18,0V13H13v8h7A1,1,0,0,0,21,20ZM21,4a1,1,0,0,0-1-1H13v8h8Z"/></svg>)svg"},
        {"layout-4-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg"><path d="M3,20V4A1,1,0,0,1,4,3h7V21H4A1,1,0,0,1,3,20Zm18,0V13H13v8h7A1,1,0,0,0,21,20ZM21,4a1,1,0,0,0-1-1H13v8h8Z"/></svg>)svg"},
        {"layout_4", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg"><path d="M3,20V4A1,1,0,0,1,4,3h7V21H4A1,1,0,0,1,3,20Zm18,0V13H13v8h7A1,1,0,0,0,21,20ZM21,4a1,1,0,0,0-1-1H13v8h8Z"/></svg>)svg"},
        {"layout-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg"><path d="M21,11H3V4A1,1,0,0,1,4,3H20a1,1,0,0,1,1,1ZM4,21h7V13H3v7A1,1,0,0,0,4,21Zm16,0a1,1,0,0,0,1-1V13H13v8Z"/></svg>)svg"},
        {"layout-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg"><path d="M21,11H3V4A1,1,0,0,1,4,3H20a1,1,0,0,1,1,1ZM4,21h7V13H3v7A1,1,0,0,0,4,21Zm16,0a1,1,0,0,0,1-1V13H13v8Z"/></svg>)svg"},
        {"left-layout-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>left-layout</title>
<path d="M0 26.016q0 2.496 1.76 4.224t4.256 1.76h20q2.464 0 4.224-1.76t1.76-4.224v-20q0-2.496-1.76-4.256t-4.224-1.76h-20q-2.496 0-4.256 1.76t-1.76 4.256v20zM4 26.016v-20q0-0.832 0.576-1.408t1.44-0.608h20q0.8 0 1.408 0.608t0.576 1.408v20q0 0.832-0.576 1.408t-1.408 0.576h-20q-0.832 0-1.44-0.576t-0.576-1.408zM8 24h6.016v-16h-6.016v16zM18.016 24h5.984v-5.984h-5.984v5.984zM18.016 14.016h5.984v-6.016h-5.984v6.016z"></path>
</svg>)svg"},
        {"left-layout-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>left-layout</title>
<path d="M0 26.016q0 2.496 1.76 4.224t4.256 1.76h20q2.464 0 4.224-1.76t1.76-4.224v-20q0-2.496-1.76-4.256t-4.224-1.76h-20q-2.496 0-4.256 1.76t-1.76 4.256v20zM4 26.016v-20q0-0.832 0.576-1.408t1.44-0.608h20q0.8 0 1.408 0.608t0.576 1.408v20q0 0.832-0.576 1.408t-1.408 0.576h-20q-0.832 0-1.44-0.576t-0.576-1.408zM8 24h6.016v-16h-6.016v16zM18.016 24h5.984v-5.984h-5.984v5.984zM18.016 14.016h5.984v-6.016h-5.984v6.016z"></path>
</svg>)svg"},
        {"left_layout", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>left-layout</title>
<path d="M0 26.016q0 2.496 1.76 4.224t4.256 1.76h20q2.464 0 4.224-1.76t1.76-4.224v-20q0-2.496-1.76-4.256t-4.224-1.76h-20q-2.496 0-4.256 1.76t-1.76 4.256v20zM4 26.016v-20q0-0.832 0.576-1.408t1.44-0.608h20q0.8 0 1.408 0.608t0.576 1.408v20q0 0.832-0.576 1.408t-1.408 0.576h-20q-0.832 0-1.44-0.576t-0.576-1.408zM8 24h6.016v-16h-6.016v16zM18.016 24h5.984v-5.984h-5.984v5.984zM18.016 14.016h5.984v-6.016h-5.984v6.016z"></path>
</svg>)svg"},
        {"link-alt-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg width="800px" height="800px" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg">
<path d="M14 10L10 14" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"/>
<path d="M16 13L18 11C19.3807 9.61929 19.3807 7.38071 18 6V6C16.6193 4.61929 14.3807 4.61929 13 6L11 8M8 11L6 13C4.61929 14.3807 4.61929 16.6193 6 18V18C7.38071 19.3807 9.61929 19.3807 11 18L13 16" stroke="currentColor" stroke-width="2" stroke-linecap="round"/>
</svg>)svg"},
        {"link-alt-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg width="800px" height="800px" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg">
<path d="M14 10L10 14" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"/>
<path d="M16 13L18 11C19.3807 9.61929 19.3807 7.38071 18 6V6C16.6193 4.61929 14.3807 4.61929 13 6L11 8M8 11L6 13C4.61929 14.3807 4.61929 16.6193 6 18V18C7.38071 19.3807 9.61929 19.3807 11 18L13 16" stroke="currentColor" stroke-width="2" stroke-linecap="round"/>
</svg>)svg"},
        {"link_alt", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg width="800px" height="800px" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg">
<path d="M14 10L10 14" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"/>
<path d="M16 13L18 11C19.3807 9.61929 19.3807 7.38071 18 6V6C16.6193 4.61929 14.3807 4.61929 13 6L11 8M8 11L6 13C4.61929 14.3807 4.61929 16.6193 6 18V18C7.38071 19.3807 9.61929 19.3807 11 18L13 16" stroke="currentColor" stroke-width="2" stroke-linecap="round"/>
</svg>)svg"},
        {"link-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg width="800px" height="800px" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg">
<path d="M10 16H7C4.79086 16 3 14.2091 3 12V12C3 9.79086 4.79086 8 7 8H10" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"/>
<path d="M16 12H8" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"/>
<path d="M14 16H17C19.2091 16 21 14.2091 21 12V12C21 9.79086 19.2091 8 17 8H14" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"/>
</svg>)svg"},
        {"link-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg width="800px" height="800px" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg">
<path d="M10 16H7C4.79086 16 3 14.2091 3 12V12C3 9.79086 4.79086 8 7 8H10" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"/>
<path d="M16 12H8" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"/>
<path d="M14 16H17C19.2091 16 21 14.2091 21 12V12C21 9.79086 19.2091 8 17 8H14" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"/>
</svg>)svg"},
        {"mark-1-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>

<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" height="800px" width="800px" version="1.1" id="Layer_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" 
	 viewBox="0 0 512 512" enable-background="new 0 0 512 512" xml:space="preserve">
<path d="M213.3,0h-128C38.2,0,0,38.2,0,85.3v128L298.7,512L512,298.7L213.3,0z M85.3,128c-23.6,0-42.7-19.1-42.7-42.7
	s19.1-42.7,42.7-42.7S128,61.8,128,85.3S108.9,128,85.3,128z M170.7,320L320,170.7l42.7,42.7L213.3,362.7L170.7,320z M256,405.3
	L405.3,256l42.7,42.7L298.7,448L256,405.3z"/>
</svg>)svg"},
        {"mark-1-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?>

<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" height="800px" width="800px" version="1.1" id="Layer_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" 
	 viewBox="0 0 512 512" enable-background="new 0 0 512 512" xml:space="preserve">
<path d="M213.3,0h-128C38.2,0,0,38.2,0,85.3v128L298.7,512L512,298.7L213.3,0z M85.3,128c-23.6,0-42.7-19.1-42.7-42.7
	s19.1-42.7,42.7-42.7S128,61.8,128,85.3S108.9,128,85.3,128z M170.7,320L320,170.7l42.7,42.7L213.3,362.7L170.7,320z M256,405.3
	L405.3,256l42.7,42.7L298.7,448L256,405.3z"/>
</svg>)svg"},
        {"mark_1", R"svg(<?xml version="1.0" encoding="utf-8"?>

<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" height="800px" width="800px" version="1.1" id="Layer_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" 
	 viewBox="0 0 512 512" enable-background="new 0 0 512 512" xml:space="preserve">
<path d="M213.3,0h-128C38.2,0,0,38.2,0,85.3v128L298.7,512L512,298.7L213.3,0z M85.3,128c-23.6,0-42.7-19.1-42.7-42.7
	s19.1-42.7,42.7-42.7S128,61.8,128,85.3S108.9,128,85.3,128z M170.7,320L320,170.7l42.7,42.7L213.3,362.7L170.7,320z M256,405.3
	L405.3,256l42.7,42.7L298.7,448L256,405.3z"/>
</svg>)svg"},
        {"mark-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>

<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" height="800px" width="800px" version="1.1" id="Layer_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" 
	 viewBox="0 0 512 512" enable-background="new 0 0 512 512" xml:space="preserve">
<path d="M213.3,0h-128C38.2,0,0,38.2,0,85.3v128L298.7,512L512,298.7L213.3,0z M85.3,128c-23.6,0-42.7-19.1-42.7-42.7
	s19.1-42.7,42.7-42.7S128,61.8,128,85.3S108.9,128,85.3,128z"/>
</svg>)svg"},
        {"mark-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?>

<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" height="800px" width="800px" version="1.1" id="Layer_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" 
	 viewBox="0 0 512 512" enable-background="new 0 0 512 512" xml:space="preserve">
<path d="M213.3,0h-128C38.2,0,0,38.2,0,85.3v128L298.7,512L512,298.7L213.3,0z M85.3,128c-23.6,0-42.7-19.1-42.7-42.7
	s19.1-42.7,42.7-42.7S128,61.8,128,85.3S108.9,128,85.3,128z"/>
</svg>)svg"},
        {"mark", R"svg(<?xml version="1.0" encoding="utf-8"?>

<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" height="800px" width="800px" version="1.1" id="Layer_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" 
	 viewBox="0 0 512 512" enable-background="new 0 0 512 512" xml:space="preserve">
<path d="M213.3,0h-128C38.2,0,0,38.2,0,85.3v128L298.7,512L512,298.7L213.3,0z M85.3,128c-23.6,0-42.7-19.1-42.7-42.7
	s19.1-42.7,42.7-42.7S128,61.8,128,85.3S108.9,128,85.3,128z"/>
</svg>)svg"},
        {"memori-card-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>

<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" height="800px" width="800px" version="1.1" id="Layer_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" 
	 viewBox="0 0 512 512" enable-background="new 0 0 512 512" xml:space="preserve">
<path d="M416.7,0H118L32.7,85.3v384c0,23.5,19.1,42.7,42.7,42.7h341.3c23.6,0,42.7-19.1,42.7-42.7V42.7C459.4,19.1,440.2,0,416.7,0z
	 M160.7,128H118V42.7h42.7V128z M224.7,128H182V42.7h42.7V128z M288.7,128H246V42.7h42.7V128z M352.7,128H310V42.7h42.7V128z
	 M416.7,128H374V42.7h42.7V128z"/>
</svg>)svg"},
        {"memori-card-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?>

<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" height="800px" width="800px" version="1.1" id="Layer_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" 
	 viewBox="0 0 512 512" enable-background="new 0 0 512 512" xml:space="preserve">
<path d="M416.7,0H118L32.7,85.3v384c0,23.5,19.1,42.7,42.7,42.7h341.3c23.6,0,42.7-19.1,42.7-42.7V42.7C459.4,19.1,440.2,0,416.7,0z
	 M160.7,128H118V42.7h42.7V128z M224.7,128H182V42.7h42.7V128z M288.7,128H246V42.7h42.7V128z M352.7,128H310V42.7h42.7V128z
	 M416.7,128H374V42.7h42.7V128z"/>
</svg>)svg"},
        {"memori_card", R"svg(<?xml version="1.0" encoding="utf-8"?>

<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" height="800px" width="800px" version="1.1" id="Layer_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" 
	 viewBox="0 0 512 512" enable-background="new 0 0 512 512" xml:space="preserve">
<path d="M416.7,0H118L32.7,85.3v384c0,23.5,19.1,42.7,42.7,42.7h341.3c23.6,0,42.7-19.1,42.7-42.7V42.7C459.4,19.1,440.2,0,416.7,0z
	 M160.7,128H118V42.7h42.7V128z M224.7,128H182V42.7h42.7V128z M288.7,128H246V42.7h42.7V128z M352.7,128H310V42.7h42.7V128z
	 M416.7,128H374V42.7h42.7V128z"/>
</svg>)svg"},
        {"minus-front-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg"><path d="M10,9H20a1,1,0,0,1,1,1V20a1,1,0,0,1-1,1H10a1,1,0,0,1-1-1V10A1,1,0,0,1,10,9ZM4,7A1,1,0,0,0,5,6V5H6A1,1,0,0,0,6,3H4A1,1,0,0,0,3,4V6A1,1,0,0,0,4,7ZM3,16a1,1,0,0,0,1,1H6a1,1,0,0,0,0-2H5V14a1,1,0,0,0-2,0ZM16,7a1,1,0,0,0,1-1V4a1,1,0,0,0-1-1H14a1,1,0,0,0,0,2h1V6A1,1,0,0,0,16,7ZM3,11a1,1,0,0,0,2,0V9A1,1,0,0,0,3,9Zm8-8H9A1,1,0,0,0,9,5h2a1,1,0,0,0,0-2Z"/></svg>)svg"},
        {"minus-front-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg"><path d="M10,9H20a1,1,0,0,1,1,1V20a1,1,0,0,1-1,1H10a1,1,0,0,1-1-1V10A1,1,0,0,1,10,9ZM4,7A1,1,0,0,0,5,6V5H6A1,1,0,0,0,6,3H4A1,1,0,0,0,3,4V6A1,1,0,0,0,4,7ZM3,16a1,1,0,0,0,1,1H6a1,1,0,0,0,0-2H5V14a1,1,0,0,0-2,0ZM16,7a1,1,0,0,0,1-1V4a1,1,0,0,0-1-1H14a1,1,0,0,0,0,2h1V6A1,1,0,0,0,16,7ZM3,11a1,1,0,0,0,2,0V9A1,1,0,0,0,3,9Zm8-8H9A1,1,0,0,0,9,5h2a1,1,0,0,0,0-2Z"/></svg>)svg"},
        {"minus_front", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg"><path d="M10,9H20a1,1,0,0,1,1,1V20a1,1,0,0,1-1,1H10a1,1,0,0,1-1-1V10A1,1,0,0,1,10,9ZM4,7A1,1,0,0,0,5,6V5H6A1,1,0,0,0,6,3H4A1,1,0,0,0,3,4V6A1,1,0,0,0,4,7ZM3,16a1,1,0,0,0,1,1H6a1,1,0,0,0,0-2H5V14a1,1,0,0,0-2,0ZM16,7a1,1,0,0,0,1-1V4a1,1,0,0,0-1-1H14a1,1,0,0,0,0,2h1V6A1,1,0,0,0,16,7ZM3,11a1,1,0,0,0,2,0V9A1,1,0,0,0,3,9Zm8-8H9A1,1,0,0,0,9,5h2a1,1,0,0,0,0-2Z"/></svg>)svg"},
        {"move-2-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>

<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" height="800px" width="800px" version="1.1" id="Layer_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" 
	 viewBox="0 0 512 512" enable-background="new 0 0 512 512" xml:space="preserve">
<path d="M209.5,0H0v209.5L81.5,128l104.7,104.7l46.5-46.5L128,81.5L209.5,0z M186.2,279.3L81.5,384L0,302.5V512h209.5L128,430.5
	l104.7-104.7L186.2,279.3z M302.5,0L384,81.5L279.3,186.2l46.5,46.5L430.5,128l81.5,81.5V0H302.5z M325.8,279.3l-46.5,46.5
	L384,430.5L302.5,512H512V302.5L430.5,384L325.8,279.3z"/>
</svg>)svg"},
        {"move-2-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?>

<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" height="800px" width="800px" version="1.1" id="Layer_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" 
	 viewBox="0 0 512 512" enable-background="new 0 0 512 512" xml:space="preserve">
<path d="M209.5,0H0v209.5L81.5,128l104.7,104.7l46.5-46.5L128,81.5L209.5,0z M186.2,279.3L81.5,384L0,302.5V512h209.5L128,430.5
	l104.7-104.7L186.2,279.3z M302.5,0L384,81.5L279.3,186.2l46.5,46.5L430.5,128l81.5,81.5V0H302.5z M325.8,279.3l-46.5,46.5
	L384,430.5L302.5,512H512V302.5L430.5,384L325.8,279.3z"/>
</svg>)svg"},
        {"move_2", R"svg(<?xml version="1.0" encoding="utf-8"?>

<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" height="800px" width="800px" version="1.1" id="Layer_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" 
	 viewBox="0 0 512 512" enable-background="new 0 0 512 512" xml:space="preserve">
<path d="M209.5,0H0v209.5L81.5,128l104.7,104.7l46.5-46.5L128,81.5L209.5,0z M186.2,279.3L81.5,384L0,302.5V512h209.5L128,430.5
	l104.7-104.7L186.2,279.3z M302.5,0L384,81.5L279.3,186.2l46.5,46.5L430.5,128l81.5,81.5V0H302.5z M325.8,279.3l-46.5,46.5
	L384,430.5L302.5,512H512V302.5L430.5,384L325.8,279.3z"/>
</svg>)svg"},
        {"os-win-01-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor"  version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink"  width="800px"
	 height="800px" viewBox="0 0 512 512" enable-background="new 0 0 512 512" xml:space="preserve">

<g id="3e91140ac1bfb9903b91c1b0ca088df9">

<path display="inline" fill-rule="evenodd" clip-rule="evenodd" d="M117.744,81.381c31.072,0,62.166,0,85.167,0
		c0,43.502,0,87.009,0,130.515c-67.472,0-134.94,0-202.41,0c0-30.973,0-61.941,0-92.91C10.986,71.688,56.308,81.381,117.744,81.381z
		 M479.421,82.483c-14.854-4.845-40.703-1.102-64.149-1.102c-60.49,0-137.605,0-192.455,0c0,65.255,0,130.515,0,195.771
		c96.226,0,192.455,0,288.683,0c0-50.879,0-101.754,0-152.634C509.741,105.301,496.347,88.005,479.421,82.483z M0.5,393.291
		c4.508,20.56,16.921,33.218,36.501,38.707c55.3,0,110.603,0,165.909,0c0-66.731,0-133.464,0-200.195c-67.472,0-134.94,0-202.41,0
		C0.5,285.631,0.5,339.459,0.5,393.291z M222.817,431.998c83.693,0,167.387,0,251.078,0c21.092-5.821,34.52-19.304,37.605-43.136
		c0-30.969,0-61.938,0-92.906c-96.229,0-192.452,0-288.683,0C222.817,341.301,222.817,386.653,222.817,431.998z">

</path>

</g>

</svg>)svg"},
        {"os-win-01-svgrepo-com", R"svg(<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor"  version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink"  width="800px"
	 height="800px" viewBox="0 0 512 512" enable-background="new 0 0 512 512" xml:space="preserve">

<g id="3e91140ac1bfb9903b91c1b0ca088df9">

<path display="inline" fill-rule="evenodd" clip-rule="evenodd" d="M117.744,81.381c31.072,0,62.166,0,85.167,0
		c0,43.502,0,87.009,0,130.515c-67.472,0-134.94,0-202.41,0c0-30.973,0-61.941,0-92.91C10.986,71.688,56.308,81.381,117.744,81.381z
		 M479.421,82.483c-14.854-4.845-40.703-1.102-64.149-1.102c-60.49,0-137.605,0-192.455,0c0,65.255,0,130.515,0,195.771
		c96.226,0,192.455,0,288.683,0c0-50.879,0-101.754,0-152.634C509.741,105.301,496.347,88.005,479.421,82.483z M0.5,393.291
		c4.508,20.56,16.921,33.218,36.501,38.707c55.3,0,110.603,0,165.909,0c0-66.731,0-133.464,0-200.195c-67.472,0-134.94,0-202.41,0
		C0.5,285.631,0.5,339.459,0.5,393.291z M222.817,431.998c83.693,0,167.387,0,251.078,0c21.092-5.821,34.52-19.304,37.605-43.136
		c0-30.969,0-61.938,0-92.906c-96.229,0-192.452,0-288.683,0C222.817,341.301,222.817,386.653,222.817,431.998z">

</path>

</g>

</svg>)svg"},
        {"os_win_01", R"svg(<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor"  version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink"  width="800px"
	 height="800px" viewBox="0 0 512 512" enable-background="new 0 0 512 512" xml:space="preserve">

<g id="3e91140ac1bfb9903b91c1b0ca088df9">

<path display="inline" fill-rule="evenodd" clip-rule="evenodd" d="M117.744,81.381c31.072,0,62.166,0,85.167,0
		c0,43.502,0,87.009,0,130.515c-67.472,0-134.94,0-202.41,0c0-30.973,0-61.941,0-92.91C10.986,71.688,56.308,81.381,117.744,81.381z
		 M479.421,82.483c-14.854-4.845-40.703-1.102-64.149-1.102c-60.49,0-137.605,0-192.455,0c0,65.255,0,130.515,0,195.771
		c96.226,0,192.455,0,288.683,0c0-50.879,0-101.754,0-152.634C509.741,105.301,496.347,88.005,479.421,82.483z M0.5,393.291
		c4.508,20.56,16.921,33.218,36.501,38.707c55.3,0,110.603,0,165.909,0c0-66.731,0-133.464,0-200.195c-67.472,0-134.94,0-202.41,0
		C0.5,285.631,0.5,339.459,0.5,393.291z M222.817,431.998c83.693,0,167.387,0,251.078,0c21.092-5.821,34.52-19.304,37.605-43.136
		c0-30.969,0-61.938,0-92.906c-96.229,0-192.452,0-288.683,0C222.817,341.301,222.817,386.653,222.817,431.998z">

</path>

</g>

</svg>)svg"},
        {"photo-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>

<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.0//EN" "http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd">
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg version="1.0" id="Layer_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" 
	 width="800px" height="800px" viewBox="0 0 64 64" enable-background="new 0 0 64 64" xml:space="preserve">
<g>
	<path fill="currentColor" d="M60,0H4C1.789,0,0,1.789,0,4v56c0,2.211,1.789,4,4,4h56c2.211,0,4-1.789,4-4V4C64,1.789,62.211,0,60,0z
		 M8,8h48v32.688l-9.113-9.113c-1.562-1.559-4.094-1.559-5.656,0L16.805,56H8V8z"/>
	<circle fill="currentColor" cx="24" cy="24" r="8"/>
</g>
</svg>)svg"},
        {"photo-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?>

<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.0//EN" "http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd">
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg version="1.0" id="Layer_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" 
	 width="800px" height="800px" viewBox="0 0 64 64" enable-background="new 0 0 64 64" xml:space="preserve">
<g>
	<path fill="currentColor" d="M60,0H4C1.789,0,0,1.789,0,4v56c0,2.211,1.789,4,4,4h56c2.211,0,4-1.789,4-4V4C64,1.789,62.211,0,60,0z
		 M8,8h48v32.688l-9.113-9.113c-1.562-1.559-4.094-1.559-5.656,0L16.805,56H8V8z"/>
	<circle fill="currentColor" cx="24" cy="24" r="8"/>
</g>
</svg>)svg"},
        {"photo", R"svg(<?xml version="1.0" encoding="utf-8"?>

<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.0//EN" "http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd">
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg version="1.0" id="Layer_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" 
	 width="800px" height="800px" viewBox="0 0 64 64" enable-background="new 0 0 64 64" xml:space="preserve">
<g>
	<path fill="currentColor" d="M60,0H4C1.789,0,0,1.789,0,4v56c0,2.211,1.789,4,4,4h56c2.211,0,4-1.789,4-4V4C64,1.789,62.211,0,60,0z
		 M8,8h48v32.688l-9.113-9.113c-1.562-1.559-4.094-1.559-5.656,0L16.805,56H8V8z"/>
	<circle fill="currentColor" cx="24" cy="24" r="8"/>
</g>
</svg>)svg"},
        {"picture-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>picture</title>
<path d="M19.571 10.857l-3.144 4.732-2.285-2.303-2.286 3.697-1.714-1.697-1.856 3.714h14.572l-3.287-8.143zM4 5v23h23v-23h-23zM25 21h-19v-14h19v14zM10.491 13.071c0.829 0 1.5-0.671 1.5-1.5s-0.671-1.5-1.5-1.5c-0.828 0-1.5 0.671-1.5 1.5s0.672 1.5 1.5 1.5z"></path>
</svg>)svg"},
        {"picture-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>picture</title>
<path d="M19.571 10.857l-3.144 4.732-2.285-2.303-2.286 3.697-1.714-1.697-1.856 3.714h14.572l-3.287-8.143zM4 5v23h23v-23h-23zM25 21h-19v-14h19v14zM10.491 13.071c0.829 0 1.5-0.671 1.5-1.5s-0.671-1.5-1.5-1.5c-0.828 0-1.5 0.671-1.5 1.5s0.672 1.5 1.5 1.5z"></path>
</svg>)svg"},
        {"picture", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>picture</title>
<path d="M19.571 10.857l-3.144 4.732-2.285-2.303-2.286 3.697-1.714-1.697-1.856 3.714h14.572l-3.287-8.143zM4 5v23h23v-23h-23zM25 21h-19v-14h19v14zM10.491 13.071c0.829 0 1.5-0.671 1.5-1.5s-0.671-1.5-1.5-1.5c-0.828 0-1.5 0.671-1.5 1.5s0.672 1.5 1.5 1.5z"></path>
</svg>)svg"},
        {"pictures1-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 -0.5 33 33" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>pictures1</title>
<path d="M26.604 29.587l-2.624-0.72-0.006-7.258 2.51 0.706 3.619-13.509-18.332-4.912-1.208 4.506h-2.068l1.863-6.952 22.193 5.946-5.947 22.193zM23.039 32h-23.039v-22.977h23.039v22.977zM21.041 11.021h-19.043v13.985h19.043v-13.985zM7.849 20.993l2.283-3.692 2.283 2.301 3.139-4.727 3.283 8.134h-14.556l1.855-3.71 1.713 1.694zM6.484 17.086c-0.828 0-1.499-0.67-1.499-1.498s0.671-1.498 1.499-1.498 1.498 0.67 1.498 1.498-0.67 1.498-1.498 1.498z"></path>
</svg>)svg"},
        {"pictures1-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 -0.5 33 33" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>pictures1</title>
<path d="M26.604 29.587l-2.624-0.72-0.006-7.258 2.51 0.706 3.619-13.509-18.332-4.912-1.208 4.506h-2.068l1.863-6.952 22.193 5.946-5.947 22.193zM23.039 32h-23.039v-22.977h23.039v22.977zM21.041 11.021h-19.043v13.985h19.043v-13.985zM7.849 20.993l2.283-3.692 2.283 2.301 3.139-4.727 3.283 8.134h-14.556l1.855-3.71 1.713 1.694zM6.484 17.086c-0.828 0-1.499-0.67-1.499-1.498s0.671-1.498 1.499-1.498 1.498 0.67 1.498 1.498-0.67 1.498-1.498 1.498z"></path>
</svg>)svg"},
        {"pictures1", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 -0.5 33 33" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>pictures1</title>
<path d="M26.604 29.587l-2.624-0.72-0.006-7.258 2.51 0.706 3.619-13.509-18.332-4.912-1.208 4.506h-2.068l1.863-6.952 22.193 5.946-5.947 22.193zM23.039 32h-23.039v-22.977h23.039v22.977zM21.041 11.021h-19.043v13.985h19.043v-13.985zM7.849 20.993l2.283-3.692 2.283 2.301 3.139-4.727 3.283 8.134h-14.556l1.855-3.71 1.713 1.694zM6.484 17.086c-0.828 0-1.499-0.67-1.499-1.498s0.671-1.498 1.499-1.498 1.498 0.67 1.498 1.498-0.67 1.498-1.498 1.498z"></path>
</svg>)svg"},
        {"pictures2-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>pictures2</title>
<path d="M28 8v16c0 1.104-0.896 2-2 2h-21c0 1.104 0.896 2 2 2h21c1.104 0 2-0.896 2-2v-16c0-1.104-0.896-2-2-2zM27 23v-16c0-1.104-0.896-2-2-2h-21c-1.104 0-2 0.896-2 2v16c0 1.104 0.896 2 2 2h21c1.104 0 2-0.896 2-2zM4 7h21v16h-21v-16zM15.627 17.311l-2.988-3.181-2.989 5.104-2.242-2.343-2.429 5.129h19.055l-4.297-11.245-4.11 6.536zM8 14c1.104 0 2-0.896 2-2s-0.896-2-2-2-2 0.896-2 2 0.896 2 2 2z"></path>
</svg>)svg"},
        {"pictures2-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>pictures2</title>
<path d="M28 8v16c0 1.104-0.896 2-2 2h-21c0 1.104 0.896 2 2 2h21c1.104 0 2-0.896 2-2v-16c0-1.104-0.896-2-2-2zM27 23v-16c0-1.104-0.896-2-2-2h-21c-1.104 0-2 0.896-2 2v16c0 1.104 0.896 2 2 2h21c1.104 0 2-0.896 2-2zM4 7h21v16h-21v-16zM15.627 17.311l-2.988-3.181-2.989 5.104-2.242-2.343-2.429 5.129h19.055l-4.297-11.245-4.11 6.536zM8 14c1.104 0 2-0.896 2-2s-0.896-2-2-2-2 0.896-2 2 0.896 2 2 2z"></path>
</svg>)svg"},
        {"pictures2", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>pictures2</title>
<path d="M28 8v16c0 1.104-0.896 2-2 2h-21c0 1.104 0.896 2 2 2h21c1.104 0 2-0.896 2-2v-16c0-1.104-0.896-2-2-2zM27 23v-16c0-1.104-0.896-2-2-2h-21c-1.104 0-2 0.896-2 2v16c0 1.104 0.896 2 2 2h21c1.104 0 2-0.896 2-2zM4 7h21v16h-21v-16zM15.627 17.311l-2.988-3.181-2.989 5.104-2.242-2.343-2.429 5.129h19.055l-4.297-11.245-4.11 6.536zM8 14c1.104 0 2-0.896 2-2s-0.896-2-2-2-2 0.896-2 2 0.896 2 2 2z"></path>
</svg>)svg"},
        {"redo-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>

<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" height="800px" width="800px" version="1.1" id="Layer_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" 
	 viewBox="0 0 512 512" enable-background="new 0 0 512 512" xml:space="preserve">
<path d="M354.5,65.1H236.3l118.2,118.2H137.8C61.7,183.2,0,244.9,0,321.1c0,76.1,61.7,137.8,137.8,137.8v-78.8
	c-32.6,0-59.1-26.4-59.1-59.1c0-32.6,26.4-59.1,59.1-59.1h216.6L236.3,380.2h118.2L512,222.6L354.5,65.1z"/>
</svg>)svg"},
        {"redo-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?>

<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" height="800px" width="800px" version="1.1" id="Layer_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" 
	 viewBox="0 0 512 512" enable-background="new 0 0 512 512" xml:space="preserve">
<path d="M354.5,65.1H236.3l118.2,118.2H137.8C61.7,183.2,0,244.9,0,321.1c0,76.1,61.7,137.8,137.8,137.8v-78.8
	c-32.6,0-59.1-26.4-59.1-59.1c0-32.6,26.4-59.1,59.1-59.1h216.6L236.3,380.2h118.2L512,222.6L354.5,65.1z"/>
</svg>)svg"},
        {"resize-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>resize</title>
<path d="M25.99 6.042l-0.004 9.735-3.732-3.733-4.454 4.455-2.665-2.665 4.454-4.454-3.384-3.383 9.785 0.045zM11.494 22.805l3.238 3.182-9.722 0.017 0.004-9.68 3.815 3.815 4.925-4.924 2.665 2.665-4.925 4.925z"></path>
</svg>)svg"},
        {"resize-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>resize</title>
<path d="M25.99 6.042l-0.004 9.735-3.732-3.733-4.454 4.455-2.665-2.665 4.454-4.454-3.384-3.383 9.785 0.045zM11.494 22.805l3.238 3.182-9.722 0.017 0.004-9.68 3.815 3.815 4.925-4.924 2.665 2.665-4.925 4.925z"></path>
</svg>)svg"},
        {"resize", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>resize</title>
<path d="M25.99 6.042l-0.004 9.735-3.732-3.733-4.454 4.455-2.665-2.665 4.454-4.454-3.384-3.383 9.785 0.045zM11.494 22.805l3.238 3.182-9.722 0.017 0.004-9.68 3.815 3.815 4.925-4.924 2.665 2.665-4.925 4.925z"></path>
</svg>)svg"},
        {"resize1-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>resize1</title>
<path d="M27.407 7.882l-2.665-2.665-4.925 4.925-3.815-3.815-0.004 9.68 9.723-0.017-3.238-3.183 4.924-4.925zM8.577 20.383l-4.453 4.453 2.665 2.666 4.453-4.455 3.732 3.732 0.004-9.734-9.784-0.045 3.383 3.383z"></path>
</svg>)svg"},
        {"resize1-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>resize1</title>
<path d="M27.407 7.882l-2.665-2.665-4.925 4.925-3.815-3.815-0.004 9.68 9.723-0.017-3.238-3.183 4.924-4.925zM8.577 20.383l-4.453 4.453 2.665 2.666 4.453-4.455 3.732 3.732 0.004-9.734-9.784-0.045 3.383 3.383z"></path>
</svg>)svg"},
        {"resize1", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>resize1</title>
<path d="M27.407 7.882l-2.665-2.665-4.925 4.925-3.815-3.815-0.004 9.68 9.723-0.017-3.238-3.183 4.924-4.925zM8.577 20.383l-4.453 4.453 2.665 2.666 4.453-4.455 3.732 3.732 0.004-9.734-9.784-0.045 3.383 3.383z"></path>
</svg>)svg"},
        {"resize2-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>resize2</title>
<path d="M23.977 28.965v-1.932h3.988v-3.988h2.057v5.92h-6.045zM27.965 5.967h-3.988v-1.932h6.045v5.92h-2.057v-3.988zM3.035 9.955h-2.056v-5.92h6.045v1.932h-3.989v3.988zM4.967 8.023h21.066v16.953h-21.066v-16.953zM7.023 23.045h16.953v-13.090h-16.953v13.090zM9.018 12.012h13.027v8.977h-13.027v-8.977zM3.035 27.033h3.988v1.932h-6.044v-5.92h2.057v3.988z"></path>
</svg>)svg"},
        {"resize2-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>resize2</title>
<path d="M23.977 28.965v-1.932h3.988v-3.988h2.057v5.92h-6.045zM27.965 5.967h-3.988v-1.932h6.045v5.92h-2.057v-3.988zM3.035 9.955h-2.056v-5.92h6.045v1.932h-3.989v3.988zM4.967 8.023h21.066v16.953h-21.066v-16.953zM7.023 23.045h16.953v-13.090h-16.953v13.090zM9.018 12.012h13.027v8.977h-13.027v-8.977zM3.035 27.033h3.988v1.932h-6.044v-5.92h2.057v3.988z"></path>
</svg>)svg"},
        {"resize2", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>resize2</title>
<path d="M23.977 28.965v-1.932h3.988v-3.988h2.057v5.92h-6.045zM27.965 5.967h-3.988v-1.932h6.045v5.92h-2.057v-3.988zM3.035 9.955h-2.056v-5.92h6.045v1.932h-3.989v3.988zM4.967 8.023h21.066v16.953h-21.066v-16.953zM7.023 23.045h16.953v-13.090h-16.953v13.090zM9.018 12.012h13.027v8.977h-13.027v-8.977zM3.035 27.033h3.988v1.932h-6.044v-5.92h2.057v3.988z"></path>
</svg>)svg"},
        {"shield-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg width="800px" height="800px" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg"><path d="M12 22c-1.148 0-3.418-1.362-5.13-3.34C4.44 15.854 3 11.967 3 7a1 1 0 0 1 .629-.929c3.274-1.31 5.88-2.613 7.816-3.903a1 1 0 0 1 1.11 0c1.935 1.29 4.543 2.594 7.816 3.903A1 1 0 0 1 21 7c0 4.968-1.44 8.855-3.87 11.66C15.419 20.637 13.149 22 12 22z" fill="currentColor"/></svg>)svg"},
        {"shield-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg width="800px" height="800px" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg"><path d="M12 22c-1.148 0-3.418-1.362-5.13-3.34C4.44 15.854 3 11.967 3 7a1 1 0 0 1 .629-.929c3.274-1.31 5.88-2.613 7.816-3.903a1 1 0 0 1 1.11 0c1.935 1.29 4.543 2.594 7.816 3.903A1 1 0 0 1 21 7c0 4.968-1.44 8.855-3.87 11.66C15.419 20.637 13.149 22 12 22z" fill="currentColor"/></svg>)svg"},
        {"standby-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>standby</title>
<path d="M2.016 18.016q0 2.848 1.088 5.44t2.976 4.448 4.48 3.008 5.44 1.088 5.44-1.088 4.48-3.008 2.976-4.448 1.12-5.44q0-4.128-2.208-7.488t-5.792-5.088v4.608q1.856 1.408 2.912 3.488t1.088 4.48q0 2.72-1.344 5.024t-3.648 3.616-5.024 1.344q-2.016 0-3.872-0.8t-3.2-2.112-2.144-3.2-0.768-3.872q0-2.4 1.056-4.48t2.944-3.488v-4.608q-3.616 1.728-5.824 5.088t-2.176 7.488zM14.016 14.016q0 0.832 0.576 1.408t1.408 0.576 1.408-0.576 0.608-1.408v-12q0-0.832-0.608-1.408t-1.408-0.608-1.408 0.608-0.576 1.408v12z"></path>
</svg>)svg"},
        {"standby-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>standby</title>
<path d="M2.016 18.016q0 2.848 1.088 5.44t2.976 4.448 4.48 3.008 5.44 1.088 5.44-1.088 4.48-3.008 2.976-4.448 1.12-5.44q0-4.128-2.208-7.488t-5.792-5.088v4.608q1.856 1.408 2.912 3.488t1.088 4.48q0 2.72-1.344 5.024t-3.648 3.616-5.024 1.344q-2.016 0-3.872-0.8t-3.2-2.112-2.144-3.2-0.768-3.872q0-2.4 1.056-4.48t2.944-3.488v-4.608q-3.616 1.728-5.824 5.088t-2.176 7.488zM14.016 14.016q0 0.832 0.576 1.408t1.408 0.576 1.408-0.576 0.608-1.408v-12q0-0.832-0.608-1.408t-1.408-0.608-1.408 0.608-0.576 1.408v12z"></path>
</svg>)svg"},
        {"substract-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>substract</title>
<path d="M0 22.016q0 0.832 0.576 1.408t1.44 0.576h5.984v6.016q0 0.832 0.576 1.408t1.44 0.576h20q0.8 0 1.408-0.576t0.576-1.408v-20q0-0.832-0.576-1.408t-1.408-0.608h-6.016v-5.984q0-0.832-0.576-1.408t-1.408-0.608h-20q-0.832 0-1.44 0.608t-0.576 1.408v20zM12 28v-16h16v16h-16z"></path>
</svg>)svg"},
        {"substract-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>substract</title>
<path d="M0 22.016q0 0.832 0.576 1.408t1.44 0.576h5.984v6.016q0 0.832 0.576 1.408t1.44 0.576h20q0.8 0 1.408-0.576t0.576-1.408v-20q0-0.832-0.576-1.408t-1.408-0.608h-6.016v-5.984q0-0.832-0.576-1.408t-1.408-0.608h-20q-0.832 0-1.44 0.608t-0.576 1.408v20zM12 28v-16h16v16h-16z"></path>
</svg>)svg"},
        {"substract", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>substract</title>
<path d="M0 22.016q0 0.832 0.576 1.408t1.44 0.576h5.984v6.016q0 0.832 0.576 1.408t1.44 0.576h20q0.8 0 1.408-0.576t0.576-1.408v-20q0-0.832-0.576-1.408t-1.408-0.608h-6.016v-5.984q0-0.832-0.576-1.408t-1.408-0.608h-20q-0.832 0-1.44 0.608t-0.576 1.408v20zM12 28v-16h16v16h-16z"></path>
</svg>)svg"},
        {"suitcase-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>suitcase</title>
<path d="M27 29h-4v-21h4c1.104 0 2 0.896 2 2v17c0 1.104-0.896 2-2 2zM10 8v0-2c0-1.105 0.896-2 2-2h7c1.104 0 2 0.895 2 2v23h-11v-21zM12 8h7c0 0 0-0.448 0-1 0-0.553-0.448-1-1-1h-5c-0.553 0-1 0.447-1 1 0 0.552 0 1 0 1zM2 27v-17c0-1.104 0.896-2 2-2h4v21h-4c-1.104 0-2-0.896-2-2z"></path>
</svg>)svg"},
        {"suitcase-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>suitcase</title>
<path d="M27 29h-4v-21h4c1.104 0 2 0.896 2 2v17c0 1.104-0.896 2-2 2zM10 8v0-2c0-1.105 0.896-2 2-2h7c1.104 0 2 0.895 2 2v23h-11v-21zM12 8h7c0 0 0-0.448 0-1 0-0.553-0.448-1-1-1h-5c-0.553 0-1 0.447-1 1 0 0.552 0 1 0 1zM2 27v-17c0-1.104 0.896-2 2-2h4v21h-4c-1.104 0-2-0.896-2-2z"></path>
</svg>)svg"},
        {"suitcase1-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>suitcase1</title>
<path d="M27 29h-23c-1.105 0-2-0.896-2-2v-12c0 0 5.221 2.685 10 3.784v1.216c0 0.553 0.447 1 1 1h5c0.552 0 1-0.447 1-1v-1.216c4.778-1.099 10-3.784 10-3.784v12c0 1.104-0.896 2-2 2zM17 17c0.552 0 1 0.447 1 1v1c0 0.553-0.448 1-1 1h-3c-0.553 0-1-0.447-1-1v-1c0-0.553 0.447-1 1-1h3zM19 17c0-0.553-0.448-1-1-1h-5c-0.553 0-1 0.447-1 1v0.896c-4.779-1.132-10-3.896-10-3.896v-4c0-1.104 0.895-2 2-2h6v-2c0-1.104 0.896-2 2-2h7c1.104 0 2 0.896 2 2v2h6c1.104 0 2 0.896 2 2v4c0 0-5.222 2.764-10 3.896v-0.896zM19 7c0-0.553-0.448-1-1-1h-5c-0.553 0-1 0.447-1 1 0 0.552 0 1 0 1h7c0 0 0-0.448 0-1z"></path>
</svg>)svg"},
        {"suitcase1-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>suitcase1</title>
<path d="M27 29h-23c-1.105 0-2-0.896-2-2v-12c0 0 5.221 2.685 10 3.784v1.216c0 0.553 0.447 1 1 1h5c0.552 0 1-0.447 1-1v-1.216c4.778-1.099 10-3.784 10-3.784v12c0 1.104-0.896 2-2 2zM17 17c0.552 0 1 0.447 1 1v1c0 0.553-0.448 1-1 1h-3c-0.553 0-1-0.447-1-1v-1c0-0.553 0.447-1 1-1h3zM19 17c0-0.553-0.448-1-1-1h-5c-0.553 0-1 0.447-1 1v0.896c-4.779-1.132-10-3.896-10-3.896v-4c0-1.104 0.895-2 2-2h6v-2c0-1.104 0.896-2 2-2h7c1.104 0 2 0.896 2 2v2h6c1.104 0 2 0.896 2 2v4c0 0-5.222 2.764-10 3.896v-0.896zM19 7c0-0.553-0.448-1-1-1h-5c-0.553 0-1 0.447-1 1 0 0.552 0 1 0 1h7c0 0 0-0.448 0-1z"></path>
</svg>)svg"},
        {"suitcase1", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>suitcase1</title>
<path d="M27 29h-23c-1.105 0-2-0.896-2-2v-12c0 0 5.221 2.685 10 3.784v1.216c0 0.553 0.447 1 1 1h5c0.552 0 1-0.447 1-1v-1.216c4.778-1.099 10-3.784 10-3.784v12c0 1.104-0.896 2-2 2zM17 17c0.552 0 1 0.447 1 1v1c0 0.553-0.448 1-1 1h-3c-0.553 0-1-0.447-1-1v-1c0-0.553 0.447-1 1-1h3zM19 17c0-0.553-0.448-1-1-1h-5c-0.553 0-1 0.447-1 1v0.896c-4.779-1.132-10-3.896-10-3.896v-4c0-1.104 0.895-2 2-2h6v-2c0-1.104 0.896-2 2-2h7c1.104 0 2 0.896 2 2v2h6c1.104 0 2 0.896 2 2v4c0 0-5.222 2.764-10 3.896v-0.896zM19 7c0-0.553-0.448-1-1-1h-5c-0.553 0-1 0.447-1 1 0 0.552 0 1 0 1h7c0 0 0-0.448 0-1z"></path>
</svg>)svg"},
        {"suitcase2-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>suitcase2</title>
<path d="M28 29h-24c-1.104 0-2-0.896-2-2v-9h4v3h4v-3h12v3h4v-3h4v9c0 1.104-0.896 2-2 2zM23 20v-4h2v4h-2zM7 20v-4h2v4h-2zM26 15h-4v2h-12v-2h-4v2h-4v-7c0-1.104 0.896-2 2-2h24c1.104 0 2 0.896 2 2v7h-4v-2zM19 4.979h-6v2.021h-2v-0.979h1v-3.021h8.021v3h0.979v1h-2v-2.021z"></path>
</svg>)svg"},
        {"suitcase2-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>suitcase2</title>
<path d="M28 29h-24c-1.104 0-2-0.896-2-2v-9h4v3h4v-3h12v3h4v-3h4v9c0 1.104-0.896 2-2 2zM23 20v-4h2v4h-2zM7 20v-4h2v4h-2zM26 15h-4v2h-12v-2h-4v2h-4v-7c0-1.104 0.896-2 2-2h24c1.104 0 2 0.896 2 2v7h-4v-2zM19 4.979h-6v2.021h-2v-0.979h1v-3.021h8.021v3h0.979v1h-2v-2.021z"></path>
</svg>)svg"},
        {"suitcase2", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>suitcase2</title>
<path d="M28 29h-24c-1.104 0-2-0.896-2-2v-9h4v3h4v-3h12v3h4v-3h4v9c0 1.104-0.896 2-2 2zM23 20v-4h2v4h-2zM7 20v-4h2v4h-2zM26 15h-4v2h-12v-2h-4v2h-4v-7c0-1.104 0.896-2 2-2h24c1.104 0 2 0.896 2 2v7h-4v-2zM19 4.979h-6v2.021h-2v-0.979h1v-3.021h8.021v3h0.979v1h-2v-2.021z"></path>
</svg>)svg"},
        {"swatches-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>swatches</title>
<path d="M0 20q0 1.952 1.12 3.488t2.88 2.144v-15.616q0-2.496 1.76-4.256t4.256-1.76h15.616q-0.608-1.76-2.144-2.88t-3.488-1.12h-13.984q-2.496 0-4.256 1.76t-1.76 4.256v13.984zM6.016 26.016q0 2.496 1.728 4.224t4.256 1.76h14.016q2.464 0 4.224-1.76t1.76-4.224v-14.016q0-2.464-1.76-4.224t-4.224-1.76h-14.016q-2.496 0-4.256 1.76t-1.728 4.224v14.016zM10.016 26.016v-14.016q0-0.832 0.576-1.408t1.408-0.576h14.016q0.8 0 1.408 0.576t0.576 1.408v8h-5.984q-0.832 0-1.44 0.608t-0.576 1.408v5.984h-8q-0.832 0-1.408-0.576t-0.576-1.408zM24 24h4l-1.984 4z"></path>
</svg>)svg"},
        {"swatches-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>swatches</title>
<path d="M0 20q0 1.952 1.12 3.488t2.88 2.144v-15.616q0-2.496 1.76-4.256t4.256-1.76h15.616q-0.608-1.76-2.144-2.88t-3.488-1.12h-13.984q-2.496 0-4.256 1.76t-1.76 4.256v13.984zM6.016 26.016q0 2.496 1.728 4.224t4.256 1.76h14.016q2.464 0 4.224-1.76t1.76-4.224v-14.016q0-2.464-1.76-4.224t-4.224-1.76h-14.016q-2.496 0-4.256 1.76t-1.728 4.224v14.016zM10.016 26.016v-14.016q0-0.832 0.576-1.408t1.408-0.576h14.016q0.8 0 1.408 0.576t0.576 1.408v8h-5.984q-0.832 0-1.44 0.608t-0.576 1.408v5.984h-8q-0.832 0-1.408-0.576t-0.576-1.408zM24 24h4l-1.984 4z"></path>
</svg>)svg"},
        {"swatches", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>swatches</title>
<path d="M0 20q0 1.952 1.12 3.488t2.88 2.144v-15.616q0-2.496 1.76-4.256t4.256-1.76h15.616q-0.608-1.76-2.144-2.88t-3.488-1.12h-13.984q-2.496 0-4.256 1.76t-1.76 4.256v13.984zM6.016 26.016q0 2.496 1.728 4.224t4.256 1.76h14.016q2.464 0 4.224-1.76t1.76-4.224v-14.016q0-2.464-1.76-4.224t-4.224-1.76h-14.016q-2.496 0-4.256 1.76t-1.728 4.224v14.016zM10.016 26.016v-14.016q0-0.832 0.576-1.408t1.408-0.576h14.016q0.8 0 1.408 0.576t0.576 1.408v8h-5.984q-0.832 0-1.44 0.608t-0.576 1.408v5.984h-8q-0.832 0-1.408-0.576t-0.576-1.408zM24 24h4l-1.984 4z"></path>
</svg>)svg"},
        {"table-left-header-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 1920 1920" xmlns="http://www.w3.org/2000/svg">
    <path d="M1694.118 0C1818.692 0 1920 101.308 1920 225.882v1468.236c0 124.574-101.308 225.882-225.882 225.882H225.882C101.308 1920 0 1818.692 0 1694.118V225.882C0 101.308 101.308 0 225.882 0h1468.236Zm.226 1355.294h-339.05v338.824h339.05v-338.824Zm-564.932 0H790.588v338.824h338.824v-338.824Zm0-564.706H790.588v338.824h338.824V790.588Zm564.819 0h-338.937v338.824h338.937V790.588Zm-564.82-564.706H790.589v338.824h338.824V225.882Zm564.707 0h-338.824v338.824h338.824V225.882Z" fill-rule="evenodd"/>
</svg>)svg"},
        {"table-left-header-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 1920 1920" xmlns="http://www.w3.org/2000/svg">
    <path d="M1694.118 0C1818.692 0 1920 101.308 1920 225.882v1468.236c0 124.574-101.308 225.882-225.882 225.882H225.882C101.308 1920 0 1818.692 0 1694.118V225.882C0 101.308 101.308 0 225.882 0h1468.236Zm.226 1355.294h-339.05v338.824h339.05v-338.824Zm-564.932 0H790.588v338.824h338.824v-338.824Zm0-564.706H790.588v338.824h338.824V790.588Zm564.819 0h-338.937v338.824h338.937V790.588Zm-564.82-564.706H790.589v338.824h338.824V225.882Zm564.707 0h-338.824v338.824h338.824V225.882Z" fill-rule="evenodd"/>
</svg>)svg"},
        {"table_left_header", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 1920 1920" xmlns="http://www.w3.org/2000/svg">
    <path d="M1694.118 0C1818.692 0 1920 101.308 1920 225.882v1468.236c0 124.574-101.308 225.882-225.882 225.882H225.882C101.308 1920 0 1818.692 0 1694.118V225.882C0 101.308 101.308 0 225.882 0h1468.236Zm.226 1355.294h-339.05v338.824h339.05v-338.824Zm-564.932 0H790.588v338.824h338.824v-338.824Zm0-564.706H790.588v338.824h338.824V790.588Zm564.819 0h-338.937v338.824h338.937V790.588Zm-564.82-564.706H790.589v338.824h338.824V225.882Zm564.707 0h-338.824v338.824h338.824V225.882Z" fill-rule="evenodd"/>
</svg>)svg"},
        {"tag-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>tag</title>
<path d="M27.395 16.112l-12.225-12.141h-11.141l-0.026 11.127 12.078 12.329c0.794 0.794 2.071 0.805 2.853 0.023l8.484-8.485c0.781-0.781 0.771-2.059-0.023-2.853zM6.982 9.004c0-1.104 0.896-2 2-2s2 0.896 2 2c0 1.105-0.896 2-2 2s-2-0.895-2-2zM17.863 22.952l-7.778-7.778 0.707-0.707 7.778 7.778-0.707 0.707zM19.984 20.831l-7.778-7.778 0.708-0.707 7.777 7.778-0.707 0.707zM22.105 18.709l-7.777-7.778 0.707-0.707 7.777 7.778-0.707 0.707z"></path>
</svg>)svg"},
        {"tag-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>tag</title>
<path d="M27.395 16.112l-12.225-12.141h-11.141l-0.026 11.127 12.078 12.329c0.794 0.794 2.071 0.805 2.853 0.023l8.484-8.485c0.781-0.781 0.771-2.059-0.023-2.853zM6.982 9.004c0-1.104 0.896-2 2-2s2 0.896 2 2c0 1.105-0.896 2-2 2s-2-0.895-2-2zM17.863 22.952l-7.778-7.778 0.707-0.707 7.778 7.778-0.707 0.707zM19.984 20.831l-7.778-7.778 0.708-0.707 7.777 7.778-0.707 0.707zM22.105 18.709l-7.777-7.778 0.707-0.707 7.777 7.778-0.707 0.707z"></path>
</svg>)svg"},
        {"tags-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>tags</title>
<path d="M25.994 16.144l-12.225-12.225-11.769 0.045 0.018 10.831 12.662 12.662c0.794 0.795 2.072 0.806 2.854 0.024l8.484-8.485c0.781-0.781 0.771-2.058-0.024-2.852zM7.081 10.952c-1.104 0-2-0.896-2-2s0.896-2 2-2c1.105 0 2 0.896 2 2s-0.895 2-2 2zM28.846 16.168l-12.225-12.225-1.471 0.005 12.27 12.207c0.795 0.795 0.805 2.071 0.023 2.853l-8.484 8.485c-0.207 0.207-0.451 0.354-0.709 0.451 0.721 0.277 1.561 0.135 2.135-0.438l8.486-8.485c0.781-0.782 0.77-2.059-0.025-2.853z"></path>
</svg>)svg"},
        {"tags-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>tags</title>
<path d="M25.994 16.144l-12.225-12.225-11.769 0.045 0.018 10.831 12.662 12.662c0.794 0.795 2.072 0.806 2.854 0.024l8.484-8.485c0.781-0.781 0.771-2.058-0.024-2.852zM7.081 10.952c-1.104 0-2-0.896-2-2s0.896-2 2-2c1.105 0 2 0.896 2 2s-0.895 2-2 2zM28.846 16.168l-12.225-12.225-1.471 0.005 12.27 12.207c0.795 0.795 0.805 2.071 0.023 2.853l-8.484 8.485c-0.207 0.207-0.451 0.354-0.709 0.451 0.721 0.277 1.561 0.135 2.135-0.438l8.486-8.485c0.781-0.782 0.77-2.059-0.025-2.853z"></path>
</svg>)svg"},
        {"tags", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>tags</title>
<path d="M25.994 16.144l-12.225-12.225-11.769 0.045 0.018 10.831 12.662 12.662c0.794 0.795 2.072 0.806 2.854 0.024l8.484-8.485c0.781-0.781 0.771-2.058-0.024-2.852zM7.081 10.952c-1.104 0-2-0.896-2-2s0.896-2 2-2c1.105 0 2 0.896 2 2s-0.895 2-2 2zM28.846 16.168l-12.225-12.225-1.471 0.005 12.27 12.207c0.795 0.795 0.805 2.071 0.023 2.853l-8.484 8.485c-0.207 0.207-0.451 0.354-0.709 0.451 0.721 0.277 1.561 0.135 2.135-0.438l8.486-8.485c0.781-0.782 0.77-2.059-0.025-2.853z"></path>
</svg>)svg"},
        {"unarchive-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 1920 1920" xmlns="http://www.w3.org/2000/svg">
    <path d="M533.333 560v160H240l.008 453.33 209.066 213.34H1470.93l209.06-213.34L1680 720h-293.33V560h352c55.96 0 101.33 45.368 101.33 101.333v511.997h16c35.35 0 64 28.66 64 64V1856c0 35.35-28.65 64-64 64H64c-35.346 0-64-28.65-64-64v-618.67c0-35.34 28.654-64 64-64h16V661.333C80 605.368 125.369 560 181.333 560h352ZM960 0l376.57 376.569-113.14 113.137L1040 306.275v867.055H879.999l-.001-867.055-183.43 183.431-113.137-113.138L960 0Z"/>
</svg>)svg"},
        {"unarchive-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 1920 1920" xmlns="http://www.w3.org/2000/svg">
    <path d="M533.333 560v160H240l.008 453.33 209.066 213.34H1470.93l209.06-213.34L1680 720h-293.33V560h352c55.96 0 101.33 45.368 101.33 101.333v511.997h16c35.35 0 64 28.66 64 64V1856c0 35.35-28.65 64-64 64H64c-35.346 0-64-28.65-64-64v-618.67c0-35.34 28.654-64 64-64h16V661.333C80 605.368 125.369 560 181.333 560h352ZM960 0l376.57 376.569-113.14 113.137L1040 306.275v867.055H879.999l-.001-867.055-183.43 183.431-113.137-113.138L960 0Z"/>
</svg>)svg"},
        {"unarchive", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 1920 1920" xmlns="http://www.w3.org/2000/svg">
    <path d="M533.333 560v160H240l.008 453.33 209.066 213.34H1470.93l209.06-213.34L1680 720h-293.33V560h352c55.96 0 101.33 45.368 101.33 101.333v511.997h16c35.35 0 64 28.66 64 64V1856c0 35.35-28.65 64-64 64H64c-35.346 0-64-28.65-64-64v-618.67c0-35.34 28.654-64 64-64h16V661.333C80 605.368 125.369 560 181.333 560h352ZM960 0l376.57 376.569-113.14 113.137L1040 306.275v867.055H879.999l-.001-867.055-183.43 183.431-113.137-113.138L960 0Z"/>
</svg>)svg"},
        {"usb-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>usb</title>
<path d="M7.929 15.586l6.364-6.364 3.203 3.203c-0.266 0.557-0.174 1.241 0.288 1.702 0.461 0.462 1.146 0.554 1.702 0.289l3.291 3.291-6.363 6.364-8.485-8.485zM19.905 12.006c-0.461-0.461-1.146-0.554-1.702-0.288l-3.203-3.203 4.949-4.949c0.781-0.781 2.048-0.781 2.828 0l5.658 5.656c0.78 0.781 0.78 2.048 0 2.828l-4.95 4.95-3.292-3.291c0.266-0.558 0.174-1.242-0.288-1.703zM8.636 29.021l-5.657-5.656 5.657-5.657 5.657 5.657-5.657 5.656zM7.266 23.319l-1.414-1.414-1.459 1.459 1.415 1.414 1.458-1.459zM10.094 26.148l-1.414-1.414-1.458 1.458 1.414 1.414 1.458-1.458z"></path>
</svg>)svg"},
        {"usb-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>usb</title>
<path d="M7.929 15.586l6.364-6.364 3.203 3.203c-0.266 0.557-0.174 1.241 0.288 1.702 0.461 0.462 1.146 0.554 1.702 0.289l3.291 3.291-6.363 6.364-8.485-8.485zM19.905 12.006c-0.461-0.461-1.146-0.554-1.702-0.288l-3.203-3.203 4.949-4.949c0.781-0.781 2.048-0.781 2.828 0l5.658 5.656c0.78 0.781 0.78 2.048 0 2.828l-4.95 4.95-3.292-3.291c0.266-0.558 0.174-1.242-0.288-1.703zM8.636 29.021l-5.657-5.656 5.657-5.657 5.657 5.657-5.657 5.656zM7.266 23.319l-1.414-1.414-1.459 1.459 1.415 1.414 1.458-1.459zM10.094 26.148l-1.414-1.414-1.458 1.458 1.414 1.414 1.458-1.458z"></path>
</svg>)svg"},
        {"usb", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>usb</title>
<path d="M7.929 15.586l6.364-6.364 3.203 3.203c-0.266 0.557-0.174 1.241 0.288 1.702 0.461 0.462 1.146 0.554 1.702 0.289l3.291 3.291-6.363 6.364-8.485-8.485zM19.905 12.006c-0.461-0.461-1.146-0.554-1.702-0.288l-3.203-3.203 4.949-4.949c0.781-0.781 2.048-0.781 2.828 0l5.658 5.656c0.78 0.781 0.78 2.048 0 2.828l-4.95 4.95-3.292-3.291c0.266-0.558 0.174-1.242-0.288-1.703zM8.636 29.021l-5.657-5.656 5.657-5.657 5.657 5.657-5.657 5.656zM7.266 23.319l-1.414-1.414-1.459 1.459 1.415 1.414 1.458-1.459zM10.094 26.148l-1.414-1.414-1.458 1.458 1.414 1.414 1.458-1.458z"></path>
</svg>)svg"},
        {"vertical-switch-alt-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg width="800px" height="800px" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg">
<path d="M8 20L7.29289 20.7071L8 21.4142L8.70711 20.7071L8 20ZM8 12L7 12L8 12ZM10 11C10.5523 11 11 10.5523 11 10C11 9.44771 10.5523 9 10 9L10 11ZM3.29289 16.7071L7.29289 20.7071L8.70711 19.2929L4.70711 15.2929L3.29289 16.7071ZM8.70711 20.7071L12.7071 16.7071L11.2929 15.2929L7.29289 19.2929L8.70711 20.7071ZM9 20L9 12L7 12L7 20L9 20ZM9 12C9 11.4477 9.44771 11 10 11L10 9C8.34315 9 7 10.3431 7 12L9 12Z" fill="currentColor"/>
<path d="M16 4L15.2929 3.29289L16 2.58579L16.7071 3.29289L16 4ZM16 12L17 12L16 12ZM14 15C13.4477 15 13 14.5523 13 14C13 13.4477 13.4477 13 14 13L14 15ZM11.2929 7.29289L15.2929 3.29289L16.7071 4.70711L12.7071 8.70711L11.2929 7.29289ZM16.7071 3.29289L20.7071 7.29289L19.2929 8.70711L15.2929 4.70711L16.7071 3.29289ZM17 4L17 12L15 12L15 4L17 4ZM17 12C17 13.6569 15.6569 15 14 15L14 13C14.5523 13 15 12.5523 15 12L17 12Z" fill="currentColor"/>
</svg>)svg"},
        {"vertical-switch-alt-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg width="800px" height="800px" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg">
<path d="M8 20L7.29289 20.7071L8 21.4142L8.70711 20.7071L8 20ZM8 12L7 12L8 12ZM10 11C10.5523 11 11 10.5523 11 10C11 9.44771 10.5523 9 10 9L10 11ZM3.29289 16.7071L7.29289 20.7071L8.70711 19.2929L4.70711 15.2929L3.29289 16.7071ZM8.70711 20.7071L12.7071 16.7071L11.2929 15.2929L7.29289 19.2929L8.70711 20.7071ZM9 20L9 12L7 12L7 20L9 20ZM9 12C9 11.4477 9.44771 11 10 11L10 9C8.34315 9 7 10.3431 7 12L9 12Z" fill="currentColor"/>
<path d="M16 4L15.2929 3.29289L16 2.58579L16.7071 3.29289L16 4ZM16 12L17 12L16 12ZM14 15C13.4477 15 13 14.5523 13 14C13 13.4477 13.4477 13 14 13L14 15ZM11.2929 7.29289L15.2929 3.29289L16.7071 4.70711L12.7071 8.70711L11.2929 7.29289ZM16.7071 3.29289L20.7071 7.29289L19.2929 8.70711L15.2929 4.70711L16.7071 3.29289ZM17 4L17 12L15 12L15 4L17 4ZM17 12C17 13.6569 15.6569 15 14 15L14 13C14.5523 13 15 12.5523 15 12L17 12Z" fill="currentColor"/>
</svg>)svg"},
        {"vertical_switch_alt", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg width="800px" height="800px" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg">
<path d="M8 20L7.29289 20.7071L8 21.4142L8.70711 20.7071L8 20ZM8 12L7 12L8 12ZM10 11C10.5523 11 11 10.5523 11 10C11 9.44771 10.5523 9 10 9L10 11ZM3.29289 16.7071L7.29289 20.7071L8.70711 19.2929L4.70711 15.2929L3.29289 16.7071ZM8.70711 20.7071L12.7071 16.7071L11.2929 15.2929L7.29289 19.2929L8.70711 20.7071ZM9 20L9 12L7 12L7 20L9 20ZM9 12C9 11.4477 9.44771 11 10 11L10 9C8.34315 9 7 10.3431 7 12L9 12Z" fill="currentColor"/>
<path d="M16 4L15.2929 3.29289L16 2.58579L16.7071 3.29289L16 4ZM16 12L17 12L16 12ZM14 15C13.4477 15 13 14.5523 13 14C13 13.4477 13.4477 13 14 13L14 15ZM11.2929 7.29289L15.2929 3.29289L16.7071 4.70711L12.7071 8.70711L11.2929 7.29289ZM16.7071 3.29289L20.7071 7.29289L19.2929 8.70711L15.2929 4.70711L16.7071 3.29289ZM17 4L17 12L15 12L15 4L17 4ZM17 12C17 13.6569 15.6569 15 14 15L14 13C14.5523 13 15 12.5523 15 12L17 12Z" fill="currentColor"/>
</svg>)svg"},
        {"website-dashboard-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg"><path d="M3,20V10H8V21H4A1,1,0,0,1,3,20ZM21,4a1,1,0,0,0-1-1H4A1,1,0,0,0,3,4V8H21ZM20,21a1,1,0,0,0,1-1V10H10V21Z"/></svg>)svg"},
        {"website-dashboard-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg"><path d="M3,20V10H8V21H4A1,1,0,0,1,3,20ZM21,4a1,1,0,0,0-1-1H4A1,1,0,0,0,3,4V8H21ZM20,21a1,1,0,0,0,1-1V10H10V21Z"/></svg>)svg"},
        {"website_dashboard", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg"><path d="M3,20V10H8V21H4A1,1,0,0,1,3,20ZM21,4a1,1,0,0,0-1-1H4A1,1,0,0,0,3,4V8H21ZM20,21a1,1,0,0,0,1-1V10H10V21Z"/></svg>)svg"},
        {"window-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>window</title>
<path d="M28 4h-24c-1.105 0-2 0.896-2 2v20c0 1.104 0.895 2 2 2h24c1.104 0 2-0.896 2-2v-20c0-1.104-0.896-2-2-2zM11.562 5.5c0.552 0 0.999 0.448 0.999 1s-0.447 1-0.999 1c-0.553 0-1-0.448-1-1s0.448-1 1-1zM8.5 5.5c0.552 0 1 0.448 1 1s-0.448 1-1 1c-0.553 0-1-0.448-1-1s0.447-1 1-1zM5.499 5.5c0.553 0 1 0.448 1 1s-0.447 1-1 1c-0.552 0-0.999-0.448-0.999-1s0.447-1 0.999-1zM28 26h-24v-16.979h24v16.979zM28 7.021h-14v-1h14v1z"></path>
</svg>)svg"},
        {"window-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>window</title>
<path d="M28 4h-24c-1.105 0-2 0.896-2 2v20c0 1.104 0.895 2 2 2h24c1.104 0 2-0.896 2-2v-20c0-1.104-0.896-2-2-2zM11.562 5.5c0.552 0 0.999 0.448 0.999 1s-0.447 1-0.999 1c-0.553 0-1-0.448-1-1s0.448-1 1-1zM8.5 5.5c0.552 0 1 0.448 1 1s-0.448 1-1 1c-0.553 0-1-0.448-1-1s0.447-1 1-1zM5.499 5.5c0.553 0 1 0.448 1 1s-0.447 1-1 1c-0.552 0-0.999-0.448-0.999-1s0.447-1 0.999-1zM28 26h-24v-16.979h24v16.979zM28 7.021h-14v-1h14v1z"></path>
</svg>)svg"},
        {"window", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>window</title>
<path d="M28 4h-24c-1.105 0-2 0.896-2 2v20c0 1.104 0.895 2 2 2h24c1.104 0 2-0.896 2-2v-20c0-1.104-0.896-2-2-2zM11.562 5.5c0.552 0 0.999 0.448 0.999 1s-0.447 1-0.999 1c-0.553 0-1-0.448-1-1s0.448-1 1-1zM8.5 5.5c0.552 0 1 0.448 1 1s-0.448 1-1 1c-0.553 0-1-0.448-1-1s0.447-1 1-1zM5.499 5.5c0.553 0 1 0.448 1 1s-0.447 1-1 1c-0.552 0-0.999-0.448-0.999-1s0.447-1 0.999-1zM28 26h-24v-16.979h24v16.979zM28 7.021h-14v-1h14v1z"></path>
</svg>)svg"},
        {"write-1-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>

<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" height="800px" width="800px" version="1.1" id="Layer_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" 
	 viewBox="0 0 512 512" enable-background="new 0 0 512 512" xml:space="preserve">
<path d="M0,242.9h170.7V72.3H0V242.9z M213.3,93.6v42.7H512V93.6H213.3z M213.3,221.6H512v-42.7H213.3V221.6z M213.3,349.6H512
	v-42.7H213.3V349.6z M213.3,434.9H512v-42.7H213.3V434.9z M0,456.3h170.7V285.6H0V456.3z"/>
</svg>)svg"},
        {"write-1-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?>

<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" height="800px" width="800px" version="1.1" id="Layer_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" 
	 viewBox="0 0 512 512" enable-background="new 0 0 512 512" xml:space="preserve">
<path d="M0,242.9h170.7V72.3H0V242.9z M213.3,93.6v42.7H512V93.6H213.3z M213.3,221.6H512v-42.7H213.3V221.6z M213.3,349.6H512
	v-42.7H213.3V349.6z M213.3,434.9H512v-42.7H213.3V434.9z M0,456.3h170.7V285.6H0V456.3z"/>
</svg>)svg"},
        {"write_1", R"svg(<?xml version="1.0" encoding="utf-8"?>

<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" height="800px" width="800px" version="1.1" id="Layer_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" 
	 viewBox="0 0 512 512" enable-background="new 0 0 512 512" xml:space="preserve">
<path d="M0,242.9h170.7V72.3H0V242.9z M213.3,93.6v42.7H512V93.6H213.3z M213.3,221.6H512v-42.7H213.3V221.6z M213.3,349.6H512
	v-42.7H213.3V349.6z M213.3,434.9H512v-42.7H213.3V434.9z M0,456.3h170.7V285.6H0V456.3z"/>
</svg>)svg"},
        {"zip-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>zip</title>
<path d="M5 30v-27h10v2h-2v2h2v2h-2v2h2v2h-2v2h2v2h2v-2h-2v-2h2v-2h-2v-2h2v-2h-2v-2h2v-2h2.991v6.009h6.009v20.991h-21zM17 18h-4v7h4v-7zM16 23.938h-2v-2h2v2zM21 3h0.245l4.755 4.755v0.183h-5v-4.938z"></path>
</svg>)svg"},
        {"zip-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>zip</title>
<path d="M5 30v-27h10v2h-2v2h2v2h-2v2h2v2h-2v2h2v2h2v-2h-2v-2h2v-2h-2v-2h2v-2h-2v-2h2v-2h2.991v6.009h6.009v20.991h-21zM17 18h-4v7h4v-7zM16 23.938h-2v-2h2v2zM21 3h0.245l4.755 4.755v0.183h-5v-4.938z"></path>
</svg>)svg"},
        {"zip", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>zip</title>
<path d="M5 30v-27h10v2h-2v2h2v2h-2v2h2v2h-2v2h2v2h2v-2h-2v-2h2v-2h-2v-2h2v-2h-2v-2h2v-2h2.991v6.009h6.009v20.991h-21zM17 18h-4v7h4v-7zM16 23.938h-2v-2h2v2zM21 3h0.245l4.755 4.755v0.183h-5v-4.938z"></path>
</svg>)svg"},
        {"zip1-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>zip1</title>
<path d="M17 24.938h-4v-7h4v7zM16 21.938h-2v2h2v-2zM17 17h-2v-2h2v2zM13 15v-2h2v2h-2zM13 9h2v2h-2v-2zM13 5h2v2h-2v-2zM17 7v2h-2v-2h2zM17 11v2h-2v-2h2zM5 30v-27h15v1.991h-13v23.018h17.018v-19.009h1.982v21h-21zM21 3h0.123l4.877 4.876v0.062h-5v-4.938z"></path>
</svg>)svg"},
        {"zip1-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>zip1</title>
<path d="M17 24.938h-4v-7h4v7zM16 21.938h-2v2h2v-2zM17 17h-2v-2h2v2zM13 15v-2h2v2h-2zM13 9h2v2h-2v-2zM13 5h2v2h-2v-2zM17 7v2h-2v-2h2zM17 11v2h-2v-2h2zM5 30v-27h15v1.991h-13v23.018h17.018v-19.009h1.982v21h-21zM21 3h0.123l4.877 4.876v0.062h-5v-4.938z"></path>
</svg>)svg"},
        {"zip1", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>zip1</title>
<path d="M17 24.938h-4v-7h4v7zM16 21.938h-2v2h2v-2zM17 17h-2v-2h2v2zM13 15v-2h2v2h-2zM13 9h2v2h-2v-2zM13 5h2v2h-2v-2zM17 7v2h-2v-2h2zM17 11v2h-2v-2h2zM5 30v-27h15v1.991h-13v23.018h17.018v-19.009h1.982v21h-21zM21 3h0.123l4.877 4.876v0.062h-5v-4.938z"></path>
</svg>)svg"},
        {"zipped-svgrepo-com.svg", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 1920 1920" xmlns="http://www.w3.org/2000/svg">
    <path d="M620.765 0v282.353c0 73.638 47.435 135.755 113.167 159.134v123.219H620.765v112.941h113.167v112.941H620.765V903.53h113.167v112.942H620.765v112.94h113.167v112.942H620.765v112.941h113.167v112.941H620.765v112.941h113.167V1920H169V0h451.765Zm564.706 0v564.706h564.705V1920H846.873v-451.765h112.715v-112.94H846.873v-112.942h112.715v-112.941H846.873V1016.47h112.715V903.529H846.873V790.59h112.715V677.646H846.873V441.261c65.506-23.379 112.715-85.496 112.715-158.908V0h225.883Zm112.94 7.454 444.537 444.31h-444.536V7.455ZM846.648 0v282.353c0 31.172-25.412 56.47-56.47 56.47-31.06 0-56.471-25.298-56.471-56.47V0h112.941Z" fill-rule="evenodd"/>
</svg>)svg"},
        {"zipped-svgrepo-com", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 1920 1920" xmlns="http://www.w3.org/2000/svg">
    <path d="M620.765 0v282.353c0 73.638 47.435 135.755 113.167 159.134v123.219H620.765v112.941h113.167v112.941H620.765V903.53h113.167v112.942H620.765v112.94h113.167v112.942H620.765v112.941h113.167v112.941H620.765v112.941h113.167V1920H169V0h451.765Zm564.706 0v564.706h564.705V1920H846.873v-451.765h112.715v-112.94H846.873v-112.942h112.715v-112.941H846.873V1016.47h112.715V903.529H846.873V790.59h112.715V677.646H846.873V441.261c65.506-23.379 112.715-85.496 112.715-158.908V0h225.883Zm112.94 7.454 444.537 444.31h-444.536V7.455ZM846.648 0v282.353c0 31.172-25.412 56.47-56.47 56.47-31.06 0-56.471-25.298-56.471-56.47V0h112.941Z" fill-rule="evenodd"/>
</svg>)svg"},
        {"zipped", R"svg(<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 1920 1920" xmlns="http://www.w3.org/2000/svg">
    <path d="M620.765 0v282.353c0 73.638 47.435 135.755 113.167 159.134v123.219H620.765v112.941h113.167v112.941H620.765V903.53h113.167v112.942H620.765v112.94h113.167v112.942H620.765v112.941h113.167v112.941H620.765v112.941h113.167V1920H169V0h451.765Zm564.706 0v564.706h564.705V1920H846.873v-451.765h112.715v-112.94H846.873v-112.942h112.715v-112.941H846.873V1016.47h112.715V903.529H846.873V790.59h112.715V677.646H846.873V441.261c65.506-23.379 112.715-85.496 112.715-158.908V0h225.883Zm112.94 7.454 444.537 444.31h-444.536V7.455ZM846.648 0v282.353c0 31.172-25.412 56.47-56.47 56.47-31.06 0-56.471-25.298-56.471-56.47V0h112.941Z" fill-rule="evenodd"/>
</svg>)svg"},
        {"split_v", R"svg(<svg width="800px" height="800px" viewBox="0 0 15 15" fill="none" xmlns="http://www.w3.org/2000/svg"><g transform="rotate(-90 7.5 7.5)"><path d="M3.23713 0.0746751C3.38454 -0.0164282 3.56861 -0.0247102 3.72361 0.0527869L11.7236 4.05279C11.931 4.15649 12.0399 4.38919 11.9866 4.61488C11.9333 4.84056 11.7319 5 11.5 5H3.5C3.22386 5 3 4.77614 3 4.5V0.500001C3 0.326712 3.08973 0.165778 3.23713 0.0746751Z" fill="currentColor"/><path d="M0 8H15V7H0V8Z" fill="currentColor"/><path d="M3.5 10C3.22386 10 3 10.2239 3 10.5V14.5C3 14.6733 3.08973 14.8342 3.23713 14.9253C3.38454 15.0164 3.56861 15.0247 3.72361 14.9472L11.7236 10.9472C11.931 10.8435 12.0399 10.6108 11.9866 10.3851C11.9333 10.1594 11.7319 10 11.5 10H3.5Z" fill="currentColor"/></g></svg>)svg"},
        {"split_vertical", R"svg(<svg width="800px" height="800px" viewBox="0 0 15 15" fill="none" xmlns="http://www.w3.org/2000/svg"><g transform="rotate(-90 7.5 7.5)"><path d="M3.23713 0.0746751C3.38454 -0.0164282 3.56861 -0.0247102 3.72361 0.0527869L11.7236 4.05279C11.931 4.15649 12.0399 4.38919 11.9866 4.61488C11.9333 4.84056 11.7319 5 11.5 5H3.5C3.22386 5 3 4.77614 3 4.5V0.500001C3 0.326712 3.08973 0.165778 3.23713 0.0746751Z" fill="currentColor"/><path d="M0 8H15V7H0V8Z" fill="currentColor"/><path d="M3.5 10C3.22386 10 3 10.2239 3 10.5V14.5C3 14.6733 3.08973 14.8342 3.23713 14.9253C3.38454 15.0164 3.56861 15.0247 3.72361 14.9472L11.7236 10.9472C11.931 10.8435 12.0399 10.6108 11.9866 10.3851C11.9333 10.1594 11.7319 10 11.5 10H3.5Z" fill="currentColor"/></g></svg>)svg"},
        {"scroll-007", R"svg(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 60 60" fill="currentColor"><path d="M56.5 46.7H3.6l26.4-30z"/></svg>)svg"},
        {"scroll_007", R"svg(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 60 60" fill="currentColor"><path d="M56.5 46.7H3.6l26.4-30z"/></svg>)svg"},
        {"scroll-006", R"svg(<svg version="1.1" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 648 648" fill="currentColor"><polygon points="28,180 619,180 323.5,504 "/></svg>)svg"},
        {"scroll_006", R"svg(<svg version="1.1" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 648 648" fill="currentColor"><polygon points="28,180 619,180 323.5,504 "/></svg>)svg"},
        {"resize2.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" version="1.1" xmlns="http://www.w3.org/2000/svg">
<title>resize2</title>
<path d="M23.977 28.965v-1.932h3.988v-3.988h2.057v5.92h-6.045zM27.965 5.967h-3.988v-1.932h6.045v5.92h-2.057v-3.988zM3.035 9.955h-2.056v-5.92h6.045v1.932h-3.989v3.988zM4.967 8.023h21.066v16.953h-21.066v-16.953zM7.023 23.045h16.953v-13.090h-16.953v13.090zM9.018 12.012h13.027v8.977h-13.027v-8.977zM3.035 27.033h3.988v1.932h-6.044v-5.92h2.057v3.988z"></path>
</svg>)svg"},
        {"quarkmeta", R"svg(<svg id="图层_1" data-name="图层 1" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 944.55854 879.97624"><defs><style>.cls-1{fill:#ff551c;}.cls-2{fill:#ff8e00;}</style></defs><title>QuarkMeta</title><path class="cls-1" d="M975.738,896.67581q-6.214,1.618-12.5374,2.8921L812.17384,929.7858c-4.22083.84413-8.426,1.56327-12.66239,2.15722a222.22593,222.22593,0,0,1-145.13336-29.405,221.91033,221.91033,0,0,1-108.34975,35.57988,222.24755,222.24755,0,0,1-89.85631-24.762,222.22988,222.22988,0,0,0,89.79382-85.99511l11.17727-19.3532,82.14944-142.28821,32.37521-56.07423a222.27865,222.27865,0,0,0,0-222.28035L545.96585,169.64925A222.1858,222.1858,0,0,0,366.31561,58.892c4.26778-.25008,8.53545-.37517,12.83443-.37517H533.17827q6.44847,0,12.85007.37517A222.236,222.236,0,0,1,725.67848,169.64925L851.3805,387.3648a222.27909,222.27909,0,0,1,0,222.28035L747.75175,789.12335l30.07716,34.2042A222.21329,222.21329,0,0,0,975.738,896.67581Z" transform="translate(-31.1795 -58.51685)"/><path class="cls-2" d="M366.36218,827.36079,240.66073,609.64515a222.27958,222.27958,0,0,1,0-222.28035L366.36218,169.64925a222.06542,222.06542,0,0,1,89.80975-85.9951A221.86068,221.86068,0,0,0,366.31561,58.892,222.26376,222.26376,0,0,0,186.64954,169.64925L60.9481,387.3648a222.35359,222.35359,0,0,0,0,222.28035L186.64954,827.36079A222.26388,222.26388,0,0,0,366.31561,938.11793a222.11029,222.11029,0,0,0,89.85632-24.762A222.06668,222.06668,0,0,1,366.36218,827.36079Z" transform="translate(-31.1795 -58.51685)"/><path class="cls-2" d="M456.17193,913.3559a222.11029,222.11029,0,0,1-89.85632,24.762c4.26768.25017,8.53536.37516,12.83433.37516h154.028q6.44845,0,12.85036-.37516A222.248,222.248,0,0,1,456.17193,913.3559Z" transform="translate(-31.1795 -58.51685)"/></svg>)svg"},
        {"arrow_down_long", R"svg(<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" xmlns="http://www.w3.org/2000/svg"><path d="M14.496 5.975l-.001 14.287-6.366-6.367L6 16.021l10.003 10.004L26 16.029 23.871 13.9l-6.366 6.368V5.977z"/></svg>)svg"},
        {"arrow_up_long", R"svg(<svg fill="currentColor" width="800px" height="800px" viewBox="0 0 32 32" xmlns="http://www.w3.org/2000/svg"><path d="M17.504 26.025l.001-14.287 6.366 6.367L26 15.979 15.997 5.975 6 15.971 8.129 18.1l6.366-6.368v14.291z"/></svg>)svg"},
        {"内容面板", R"svg(<svg id="图层_1" data-name="图层 1" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 1000.00012 404.19"><defs><style>.cls-1{fill:#fff;}</style></defs><title>内容面板</title><path d="M-.00006,335.50608V664.49392A37.60108,37.60108,0,0,0,37.601,702.095H962.399a37.60108,37.60108,0,0,0,37.60108-37.60108V335.50608A37.60108,37.60108,0,0,0,962.399,297.905H37.601A37.60108,37.60108,0,0,0-.00006,335.50608Zm62.74,266.24785V398.23606A37.60108,37.60108,0,0,1,100.341,360.635h49.51793A37.60108,37.60108,0,0,1,187.46,398.23606V601.75393A37.60108,37.60108,0,0,1,149.85894,639.355H100.341A37.60108,37.60108,0,0,1,62.73993,601.75393Zm274.569,37.60108h-49.5178A37.60109,37.60109,0,0,1,250.19,601.75393V398.23606A37.60109,37.60109,0,0,1,287.79109,360.635h49.5178A37.60108,37.60108,0,0,1,374.91,398.23606V601.75393A37.60108,37.60108,0,0,1,337.30889,639.355Zm187.44995,0H475.241A37.60109,37.60109,0,0,1,437.64,601.75393V398.23606A37.60109,37.60109,0,0,1,475.241,360.635h49.5178a37.60108,37.60108,0,0,1,37.60108,37.60108V601.75393A37.60108,37.60108,0,0,1,524.75884,639.355Zm187.46,0H662.701a37.60109,37.60109,0,0,1-37.60109-37.60108V398.23606A37.60109,37.60109,0,0,1,662.701,360.635h49.5178a37.60109,37.60109,0,0,1,37.60109,37.60108V601.75393A37.60109,37.60109,0,0,1,712.2188,639.355Zm187.4502,0H850.151a37.60108,37.60108,0,0,1-37.60108-37.60108V398.23606A37.60108,37.60108,0,0,1,850.151,360.635H899.669a37.60108,37.60108,0,0,1,37.60108,37.60108V601.75393A37.60108,37.60108,0,0,1,899.669,639.355Z" transform="translate(0.00006 -297.905)"/><path class="cls-1" d="M899.66873,360.635H100.341A37.601,37.601,0,0,0,62.74,398.23608V601.75391A37.601,37.601,0,0,0,100.341,639.355H899.66873a37.60123,37.60123,0,0,0,37.60132-37.60107V398.23608A37.60123,37.60123,0,0,0,899.66873,360.635Z" transform="translate(0.00006 -297.905)"/></svg>)svg"},
        {"内容面板.svg", R"svg(<svg id="图层_1" data-name="图层 1" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 1000.00012 404.19"><defs><style>.cls-1{fill:#fff;}</style></defs><title>内容面板</title><path d="M-.00006,335.50608V664.49392A37.60108,37.60108,0,0,0,37.601,702.095H962.399a37.60108,37.60108,0,0,0,37.60108-37.60108V335.50608A37.60108,37.60108,0,0,0,962.399,297.905H37.601A37.60108,37.60108,0,0,0-.00006,335.50608Zm62.74,266.24785V398.23606A37.60108,37.60108,0,0,1,100.341,360.635h49.51793A37.60108,37.60108,0,0,1,187.46,398.23606V601.75393A37.60108,37.60108,0,0,1,149.85894,639.355H100.341A37.60108,37.60108,0,0,1,62.73993,601.75393Zm274.569,37.60108h-49.5178A37.60109,37.60109,0,0,1,250.19,601.75393V398.23606A37.60109,37.60109,0,0,1,287.79109,360.635h49.5178A37.60108,37.60108,0,0,1,374.91,398.23606V601.75393A37.60108,37.60108,0,0,1,337.30889,639.355Zm187.44995,0H475.241A37.60109,37.60109,0,0,1,437.64,601.75393V398.23606A37.60109,37.60109,0,0,1,475.241,360.635h49.5178a37.60108,37.60108,0,0,1,37.60108,37.60108V601.75393A37.60108,37.60108,0,0,1,524.75884,639.355Zm187.46,0H662.701a37.60109,37.60109,0,0,1-37.60109-37.60108V398.23606A37.60109,37.60109,0,0,1,662.701,360.635h49.5178a37.60109,37.60109,0,0,1,37.60109,37.60108V601.75393A37.60109,37.60109,0,0,1,712.2188,639.355Zm187.4502,0H850.151a37.60108,37.60108,0,0,1-37.60108-37.60108V398.23606A37.60108,37.60108,0,0,1,850.151,360.635H899.669a37.60108,37.60108,0,0,1,37.60108,37.60108V601.75393A37.60108,37.60108,0,0,1,899.669,639.355Z" transform="translate(0.00006 -297.905)"/><path class="cls-1" d="M899.66873,360.635H100.341A37.601,37.601,0,0,0,62.74,398.23608V601.75391A37.601,37.601,0,0,0,100.341,639.355H899.66873a37.60123,37.60123,0,0,0,37.60132-37.60107V398.23608A37.60123,37.60123,0,0,0,899.66873,360.635Z" transform="translate(0.00006 -297.905)"/></svg>)svg"},
        {"显示收藏栏+内容面板+筛选器", R"svg(<svg id="图层_1" data-name="图层 1" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 1000.00012 404.19"><defs><style>.cls-1{fill:#fff;}</style></defs><title>显示收藏栏+内容面板+筛选器</title><rect class="cls-1" x="62.73999" y="62.72998" width="124.72009" height="278.72003"/><rect class="cls-1" x="250.19" y="62.72998" width="124.72009" height="278.72003"/><rect class="cls-1" x="437.64001" y="62.72998" width="124.72009" height="278.72003"/><rect class="cls-1" x="625.09003" y="62.72998" width="124.72009" height="278.72003"/><rect class="cls-1" x="812.54004" y="62.72998" width="124.72009" height="278.72003"/><path d="M.14061,335.26052V664.24835A37.60109,37.60109,0,0,0,37.7417,701.84944h924.798a37.60109,37.60109,0,0,0,37.60109-37.60109V335.26052a37.60109,37.60109,0,0,0-37.60109-37.60109H37.7417A37.60109,37.60109,0,0,0,.14061,335.26052Zm62.74,266.24784V397.9905a37.60109,37.60109,0,0,1,37.60109-37.60109h49.51792A37.60109,37.60109,0,0,1,187.6007,397.9905V601.50836a37.60109,37.60109,0,0,1-37.60109,37.60109H100.48169A37.60109,37.60109,0,0,1,62.8806,601.50836Zm274.569,37.60109h-49.5178a37.60109,37.60109,0,0,1-37.60108-37.60109V397.9905a37.60109,37.60109,0,0,1,37.60108-37.60109h49.5178a37.60109,37.60109,0,0,1,37.60109,37.60109V601.50836A37.60109,37.60109,0,0,1,337.44956,639.10945Zm187.44995,0h-49.5178a37.60109,37.60109,0,0,1-37.60108-37.60109V397.9905a37.60109,37.60109,0,0,1,37.60108-37.60109h49.5178A37.60109,37.60109,0,0,1,562.5006,397.9905V601.50836A37.60109,37.60109,0,0,1,524.89951,639.10945Zm187.46,0H662.84167a37.60109,37.60109,0,0,1-37.60108-37.60109V397.9905a37.60109,37.60109,0,0,1,37.60108-37.60109h49.51781a37.60109,37.60109,0,0,1,37.60108,37.60109V601.50836A37.60109,37.60109,0,0,1,712.35948,639.10945Zm187.45019,0h-49.518a37.60109,37.60109,0,0,1-37.60109-37.60109V397.9905a37.60109,37.60109,0,0,1,37.60109-37.60109h49.518a37.60109,37.60109,0,0,1,37.60109,37.60109V601.50836A37.60109,37.60109,0,0,1,899.80967,639.10945Z" transform="translate(-0.14061 -297.65943)"/><path class="cls-1" d="M711.699,360.64H287.93169a37.60093,37.60093,0,0,0-37.60107,37.601V601.75888A37.601,37.601,0,0,0,287.93169,639.36H711.699a37.601,37.601,0,0,0,37.60107-37.60107V398.24106A37.60093,37.60093,0,0,0,711.699,360.64Z" transform="translate(-0.14061 -297.65943)"/></svg>)svg"},
        {"显示收藏栏+内容面板+筛选器.svg", R"svg(<svg id="图层_1" data-name="图层 1" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 1000.00012 404.19"><defs><style>.cls-1{fill:#fff;}</style></defs><title>显示收藏栏+内容面板+筛选器</title><rect class="cls-1" x="62.73999" y="62.72998" width="124.72009" height="278.72003"/><rect class="cls-1" x="250.19" y="62.72998" width="124.72009" height="278.72003"/><rect class="cls-1" x="437.64001" y="62.72998" width="124.72009" height="278.72003"/><rect class="cls-1" x="625.09003" y="62.72998" width="124.72009" height="278.72003"/><rect class="cls-1" x="812.54004" y="62.72998" width="124.72009" height="278.72003"/><path d="M.14061,335.26052V664.24835A37.60109,37.60109,0,0,0,37.7417,701.84944h924.798a37.60109,37.60109,0,0,0,37.60109-37.60109V335.26052a37.60109,37.60109,0,0,0-37.60109-37.60109H37.7417A37.60109,37.60109,0,0,0,.14061,335.26052Zm62.74,266.24784V397.9905a37.60109,37.60109,0,0,1,37.60109-37.60109h49.51792A37.60109,37.60109,0,0,1,187.6007,397.9905V601.50836a37.60109,37.60109,0,0,1-37.60109,37.60109H100.48169A37.60109,37.60109,0,0,1,62.8806,601.50836Zm274.569,37.60109h-49.5178a37.60109,37.60109,0,0,1-37.60108-37.60109V397.9905a37.60109,37.60109,0,0,1,37.60108-37.60109h49.5178a37.60109,37.60109,0,0,1,37.60109,37.60109V601.50836A37.60109,37.60109,0,0,1,337.44956,639.10945Zm187.44995,0h-49.5178a37.60109,37.60109,0,0,1-37.60108-37.60109V397.9905a37.60109,37.60109,0,0,1,37.60108-37.60109h49.5178A37.60109,37.60109,0,0,1,562.5006,397.9905V601.50836A37.60109,37.60109,0,0,1,524.89951,639.10945Zm187.46,0H662.84167a37.60109,37.60109,0,0,1-37.60108-37.60109V397.9905a37.60109,37.60109,0,0,1,37.60108-37.60109h49.51781a37.60109,37.60109,0,0,1,37.60108,37.60109V601.50836A37.60109,37.60109,0,0,1,712.35948,639.10945Zm187.45019,0h-49.518a37.60109,37.60109,0,0,1-37.60109-37.60109V397.9905a37.60109,37.60109,0,0,1,37.60109-37.60109h49.518a37.60109,37.60109,0,0,1,37.60109,37.60109V601.50836A37.60109,37.60109,0,0,1,899.80967,639.10945Z" transform="translate(-0.14061 -297.65943)"/><path class="cls-1" d="M711.699,360.64H287.93169a37.60093,37.60093,0,0,0-37.60107,37.601V601.75888A37.601,37.601,0,0,0,287.93169,639.36H711.699a37.601,37.601,0,0,0,37.60107-37.60107V398.24106A37.60093,37.60093,0,0,0,711.699,360.64Z" transform="translate(-0.14061 -297.65943)"/></svg>)svg"},
        {"重置分栏", R"svg(<svg id="图层_1" data-name="图层 1" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 1000.00012 404.19"><defs><style>.cls-1{fill:#fff;}</style></defs><title>重置分栏</title><rect class="cls-1" x="62.73999" y="62.72998" width="124.72009" height="278.72003"/><rect class="cls-1" x="250.19" y="62.72998" width="124.72009" height="278.72003"/><rect class="cls-1" x="437.64001" y="62.72998" width="124.72009" height="278.72003"/><rect class="cls-1" x="625.09003" y="62.72998" width="124.72009" height="278.72003"/><rect class="cls-1" x="812.54004" y="62.72998" width="124.72009" height="278.72003"/><path d="M2.14061,338.26052V667.24835A37.60109,37.60109,0,0,0,39.7417,704.84944h924.798a37.60109,37.60109,0,0,0,37.60109-37.60109V338.26052a37.60109,37.60109,0,0,0-37.60109-37.60109H39.7417A37.60109,37.60109,0,0,0,2.14061,338.26052Zm62.74,266.24784V400.9905a37.60109,37.60109,0,0,1,37.60109-37.60109h49.51792A37.60109,37.60109,0,0,1,189.6007,400.9905V604.50836a37.60109,37.60109,0,0,1-37.60109,37.60109H102.48169A37.60109,37.60109,0,0,1,64.8806,604.50836Zm274.569,37.60109h-49.5178a37.60109,37.60109,0,0,1-37.60108-37.60109V400.9905a37.60109,37.60109,0,0,1,37.60108-37.60109h49.5178a37.60109,37.60109,0,0,1,37.60109,37.60109V604.50836A37.60109,37.60109,0,0,1,339.44956,642.10945Zm187.44995,0h-49.5178a37.60109,37.60109,0,0,1-37.60108-37.60109V400.9905a37.60109,37.60109,0,0,1,37.60108-37.60109h49.5178A37.60109,37.60109,0,0,1,564.5006,400.9905V604.50836A37.60109,37.60109,0,0,1,526.89951,642.10945Zm187.46,0H664.84167a37.60109,37.60109,0,0,1-37.60108-37.60109V400.9905a37.60109,37.60109,0,0,1,37.60108-37.60109h49.51781a37.60109,37.60109,0,0,1,37.60108,37.60109V604.50836A37.60109,37.60109,0,0,1,714.35948,642.10945Zm187.45019,0h-49.518a37.60109,37.60109,0,0,1-37.60109-37.60109V400.9905a37.60109,37.60109,0,0,1,37.60109-37.60109h49.518a37.60109,37.60109,0,0,1,37.60109,37.60109V604.50836A37.60109,37.60109,0,0,1,901.80967,642.10945Z" transform="translate(-2.14061 -300.65943)"/></svg>)svg"},
        {"重置分栏.svg", R"svg(<svg id="图层_1" data-name="图层 1" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 1000.00012 404.19"><defs><style>.cls-1{fill:#fff;}</style></defs><title>重置分栏</title><rect class="cls-1" x="62.73999" y="62.72998" width="124.72009" height="278.72003"/><rect class="cls-1" x="250.19" y="62.72998" width="124.72009" height="278.72003"/><rect class="cls-1" x="437.64001" y="62.72998" width="124.72009" height="278.72003"/><rect class="cls-1" x="625.09003" y="62.72998" width="124.72009" height="278.72003"/><rect class="cls-1" x="812.54004" y="62.72998" width="124.72009" height="278.72003"/><path d="M2.14061,338.26052V667.24835A37.60109,37.60109,0,0,0,39.7417,704.84944h924.798a37.60109,37.60109,0,0,0,37.60109-37.60109V338.26052a37.60109,37.60109,0,0,0-37.60109-37.60109H39.7417A37.60109,37.60109,0,0,0,2.14061,338.26052Zm62.74,266.24784V400.9905a37.60109,37.60109,0,0,1,37.60109-37.60109h49.51792A37.60109,37.60109,0,0,1,189.6007,400.9905V604.50836a37.60109,37.60109,0,0,1-37.60109,37.60109H102.48169A37.60109,37.60109,0,0,1,64.8806,604.50836Zm274.569,37.60109h-49.5178a37.60109,37.60109,0,0,1-37.60108-37.60109V400.9905a37.60109,37.60109,0,0,1,37.60108-37.60109h49.5178a37.60109,37.60109,0,0,1,37.60109,37.60109V604.50836A37.60109,37.60109,0,0,1,339.44956,642.10945Zm187.44995,0h-49.5178a37.60109,37.60109,0,0,1-37.60108-37.60109V400.9905a37.60109,37.60109,0,0,1,37.60108-37.60109h49.5178A37.60109,37.60109,0,0,1,564.5006,400.9905V604.50836A37.60109,37.60109,0,0,1,526.89951,642.10945Zm187.46,0H664.84167a37.60109,37.60109,0,0,1-37.60108-37.60109V400.9905a37.60109,37.60109,0,0,1,37.60108-37.60109h49.51781a37.60109,37.60109,0,0,1,37.60108,37.60109V604.50836A37.60109,37.60109,0,0,1,714.35948,642.10945Zm187.45019,0h-49.518a37.60109,37.60109,0,0,1-37.60109-37.60109V400.9905a37.60109,37.60109,0,0,1,37.60109-37.60109h49.518a37.60109,37.60109,0,0,1,37.60109,37.60109V604.50836A37.60109,37.60109,0,0,1,901.80967,642.10945Z" transform="translate(-2.14061 -300.65943)"/></svg>)svg"},
        {"隐藏元数据面板", R"svg(<svg id="图层_1" data-name="图层 1" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 1000.00012 404.19"><defs><style>.cls-1{fill:#fff;}</style></defs><title>隐藏元数据面板</title><rect class="cls-1" x="62.73999" y="62.72998" width="124.72009" height="278.72003"/><rect class="cls-1" x="250.19" y="62.72998" width="124.72009" height="278.72003"/><rect class="cls-1" x="437.64001" y="62.72998" width="124.72009" height="278.72003"/><rect class="cls-1" x="812.54004" y="62.72998" width="124.72009" height="278.72003"/><path d="M-.00006,335.50608V664.49392A37.60108,37.60108,0,0,0,37.601,702.095H962.399a37.60108,37.60108,0,0,0,37.60108-37.60108V335.50608A37.60108,37.60108,0,0,0,962.399,297.905H37.601A37.60108,37.60108,0,0,0-.00006,335.50608Zm62.74,266.24785V398.23606A37.60108,37.60108,0,0,1,100.341,360.635h49.51793A37.60108,37.60108,0,0,1,187.46,398.23606V601.75393A37.60108,37.60108,0,0,1,149.85894,639.355H100.341A37.60108,37.60108,0,0,1,62.73993,601.75393Zm274.569,37.60108h-49.5178A37.60109,37.60109,0,0,1,250.19,601.75393V398.23606A37.60109,37.60109,0,0,1,287.79109,360.635h49.5178A37.60108,37.60108,0,0,1,374.91,398.23606V601.75393A37.60108,37.60108,0,0,1,337.30889,639.355Zm187.44995,0H475.241A37.60109,37.60109,0,0,1,437.64,601.75393V398.23606A37.60109,37.60109,0,0,1,475.241,360.635h49.5178a37.60108,37.60108,0,0,1,37.60108,37.60108V601.75393A37.60108,37.60108,0,0,1,524.75884,639.355Zm187.46,0H662.701a37.60109,37.60109,0,0,1-37.60109-37.60108V398.23606A37.60109,37.60109,0,0,1,662.701,360.635h49.5178a37.60109,37.60109,0,0,1,37.60109,37.60108V601.75393A37.60109,37.60109,0,0,1,712.2188,639.355Zm187.4502,0H850.151a37.60108,37.60108,0,0,1-37.60108-37.60108V398.23606A37.60108,37.60108,0,0,1,850.151,360.635H899.669a37.60108,37.60108,0,0,1,37.60108,37.60108V601.75393A37.60108,37.60108,0,0,1,899.669,639.355Z" transform="translate(0.00006 -297.905)"/></svg>)svg"},
        {"隐藏元数据面板.svg", R"svg(<svg id="图层_1" data-name="图层 1" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 1000.00012 404.19"><defs><style>.cls-1{fill:#fff;}</style></defs><title>隐藏元数据面板</title><rect class="cls-1" x="62.73999" y="62.72998" width="124.72009" height="278.72003"/><rect class="cls-1" x="250.19" y="62.72998" width="124.72009" height="278.72003"/><rect class="cls-1" x="437.64001" y="62.72998" width="124.72009" height="278.72003"/><rect class="cls-1" x="812.54004" y="62.72998" width="124.72009" height="278.72003"/><path d="M-.00006,335.50608V664.49392A37.60108,37.60108,0,0,0,37.601,702.095H962.399a37.60108,37.60108,0,0,0,37.60108-37.60108V335.50608A37.60108,37.60108,0,0,0,962.399,297.905H37.601A37.60108,37.60108,0,0,0-.00006,335.50608Zm62.74,266.24785V398.23606A37.60108,37.60108,0,0,1,100.341,360.635h49.51793A37.60108,37.60108,0,0,1,187.46,398.23606V601.75393A37.60108,37.60108,0,0,1,149.85894,639.355H100.341A37.60108,37.60108,0,0,1,62.73993,601.75393Zm274.569,37.60108h-49.5178A37.60109,37.60109,0,0,1,250.19,601.75393V398.23606A37.60109,37.60109,0,0,1,287.79109,360.635h49.5178A37.60108,37.60108,0,0,1,374.91,398.23606V601.75393A37.60108,37.60108,0,0,1,337.30889,639.355Zm187.44995,0H475.241A37.60109,37.60109,0,0,1,437.64,601.75393V398.23606A37.60109,37.60109,0,0,1,475.241,360.635h49.5178a37.60108,37.60108,0,0,1,37.60108,37.60108V601.75393A37.60108,37.60108,0,0,1,524.75884,639.355Zm187.46,0H662.701a37.60109,37.60109,0,0,1-37.60109-37.60108V398.23606A37.60109,37.60109,0,0,1,662.701,360.635h49.5178a37.60109,37.60109,0,0,1,37.60109,37.60108V601.75393A37.60109,37.60109,0,0,1,712.2188,639.355Zm187.4502,0H850.151a37.60108,37.60108,0,0,1-37.60108-37.60108V398.23606A37.60108,37.60108,0,0,1,850.151,360.635H899.669a37.60108,37.60108,0,0,1,37.60108,37.60108V601.75393A37.60108,37.60108,0,0,1,899.669,639.355Z" transform="translate(0.00006 -297.905)"/></svg>)svg"},
        {"隐藏收藏栏", R"svg(<svg id="图层_1" data-name="图层 1" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 1000.00012 404.19"><defs><style>.cls-1{fill:#fff;}</style></defs><title>隐藏收藏栏</title><rect class="cls-1" x="62.73999" y="62.72998" width="124.72009" height="278.72003"/><rect class="cls-1" x="437.64001" y="62.72998" width="124.72009" height="278.72003"/><rect class="cls-1" x="625.09003" y="62.72998" width="124.72009" height="278.72003"/><rect class="cls-1" x="812.54004" y="62.72998" width="124.72009" height="278.72003"/><path d="M-.00006,335.50608V664.49392A37.60108,37.60108,0,0,0,37.601,702.095H962.399a37.60108,37.60108,0,0,0,37.60108-37.60108V335.50608A37.60108,37.60108,0,0,0,962.399,297.905H37.601A37.60108,37.60108,0,0,0-.00006,335.50608Zm62.74,266.24785V398.23606A37.60108,37.60108,0,0,1,100.341,360.635h49.51793A37.60108,37.60108,0,0,1,187.46,398.23606V601.75393A37.60108,37.60108,0,0,1,149.85894,639.355H100.341A37.60108,37.60108,0,0,1,62.73993,601.75393Zm274.569,37.60108h-49.5178A37.60109,37.60109,0,0,1,250.19,601.75393V398.23606A37.60109,37.60109,0,0,1,287.79109,360.635h49.5178A37.60108,37.60108,0,0,1,374.91,398.23606V601.75393A37.60108,37.60108,0,0,1,337.30889,639.355Zm187.44995,0H475.241A37.60109,37.60109,0,0,1,437.64,601.75393V398.23606A37.60109,37.60109,0,0,1,475.241,360.635h49.5178a37.60108,37.60108,0,0,1,37.60108,37.60108V601.75393A37.60108,37.60108,0,0,1,524.75884,639.355Zm187.46,0H662.701a37.60109,37.60109,0,0,1-37.60109-37.60108V398.23606A37.60109,37.60109,0,0,1,662.701,360.635h49.5178a37.60109,37.60109,0,0,1,37.60109,37.60108V601.75393A37.60109,37.60109,0,0,1,712.2188,639.355Zm187.4502,0H850.151a37.60108,37.60108,0,0,1-37.60108-37.60108V398.23606A37.60108,37.60108,0,0,1,850.151,360.635H899.669a37.60108,37.60108,0,0,1,37.60108,37.60108V601.75393A37.60108,37.60108,0,0,1,899.669,639.355Z" transform="translate(0.00006 -297.905)"/></svg>)svg"},
        {"隐藏收藏栏.svg", R"svg(<svg id="图层_1" data-name="图层 1" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 1000.00012 404.19"><defs><style>.cls-1{fill:#fff;}</style></defs><title>隐藏收藏栏</title><rect class="cls-1" x="62.73999" y="62.72998" width="124.72009" height="278.72003"/><rect class="cls-1" x="437.64001" y="62.72998" width="124.72009" height="278.72003"/><rect class="cls-1" x="625.09003" y="62.72998" width="124.72009" height="278.72003"/><rect class="cls-1" x="812.54004" y="62.72998" width="124.72009" height="278.72003"/><path d="M-.00006,335.50608V664.49392A37.60108,37.60108,0,0,0,37.601,702.095H962.399a37.60108,37.60108,0,0,0,37.60108-37.60108V335.50608A37.60108,37.60108,0,0,0,962.399,297.905H37.601A37.60108,37.60108,0,0,0-.00006,335.50608Zm62.74,266.24785V398.23606A37.60108,37.60108,0,0,1,100.341,360.635h49.51793A37.60108,37.60108,0,0,1,187.46,398.23606V601.75393A37.60108,37.60108,0,0,1,149.85894,639.355H100.341A37.60108,37.60108,0,0,1,62.73993,601.75393Zm274.569,37.60108h-49.5178A37.60109,37.60109,0,0,1,250.19,601.75393V398.23606A37.60109,37.60109,0,0,1,287.79109,360.635h49.5178A37.60108,37.60108,0,0,1,374.91,398.23606V601.75393A37.60108,37.60108,0,0,1,337.30889,639.355Zm187.44995,0H475.241A37.60109,37.60109,0,0,1,437.64,601.75393V398.23606A37.60109,37.60109,0,0,1,475.241,360.635h49.5178a37.60108,37.60108,0,0,1,37.60108,37.60108V601.75393A37.60108,37.60108,0,0,1,524.75884,639.355Zm187.46,0H662.701a37.60109,37.60109,0,0,1-37.60109-37.60108V398.23606A37.60109,37.60109,0,0,1,662.701,360.635h49.5178a37.60109,37.60109,0,0,1,37.60109,37.60108V601.75393A37.60109,37.60109,0,0,1,712.2188,639.355Zm187.4502,0H850.151a37.60108,37.60108,0,0,1-37.60108-37.60108V398.23606A37.60108,37.60108,0,0,1,850.151,360.635H899.669a37.60108,37.60108,0,0,1,37.60108,37.60108V601.75393A37.60108,37.60108,0,0,1,899.669,639.355Z" transform="translate(0.00006 -297.905)"/></svg>)svg"},
        {"隐藏目录导航", R"svg(<svg id="图层_1" data-name="图层 1" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 1000.00012 404.19"><defs><style>.cls-1{fill:#fff;}</style></defs><title>隐藏目录导航</title><rect class="cls-1" x="250.19" y="62.72998" width="124.72009" height="278.72003"/><rect class="cls-1" x="437.64001" y="62.72998" width="124.72009" height="278.72003"/><rect class="cls-1" x="625.09003" y="62.72998" width="124.72009" height="278.72003"/><rect class="cls-1" x="812.54004" y="62.72998" width="124.72009" height="278.72003"/><path d="M-.00006,335.50608V664.49392A37.60108,37.60108,0,0,0,37.601,702.095H962.399a37.60108,37.60108,0,0,0,37.60108-37.60108V335.50608A37.60108,37.60108,0,0,0,962.399,297.905H37.601A37.60108,37.60108,0,0,0-.00006,335.50608Zm62.74,266.24785V398.23606A37.60108,37.60108,0,0,1,100.341,360.635h49.51793A37.60108,37.60108,0,0,1,187.46,398.23606V601.75393A37.60108,37.60108,0,0,1,149.85894,639.355H100.341A37.60108,37.60108,0,0,1,62.73993,601.75393Zm274.569,37.60108h-49.5178A37.60109,37.60109,0,0,1,250.19,601.75393V398.23606A37.60109,37.60109,0,0,1,287.79109,360.635h49.5178A37.60108,37.60108,0,0,1,374.91,398.23606V601.75393A37.60108,37.60108,0,0,1,337.30889,639.355Zm187.44995,0H475.241A37.60109,37.60109,0,0,1,437.64,601.75393V398.23606A37.60109,37.60109,0,0,1,475.241,360.635h49.5178a37.60108,37.60108,0,0,1,37.60108,37.60108V601.75393A37.60108,37.60108,0,0,1,524.75884,639.355Zm187.46,0H662.701a37.60109,37.60109,0,0,1-37.60109-37.60108V398.23606A37.60109,37.60109,0,0,1,662.701,360.635h49.5178a37.60109,37.60109,0,0,1,37.60109,37.60108V601.75393A37.60109,37.60109,0,0,1,712.2188,639.355Zm187.4502,0H850.151a37.60108,37.60108,0,0,1-37.60108-37.60108V398.23606A37.60108,37.60108,0,0,1,850.151,360.635H899.669a37.60108,37.60108,0,0,1,37.60108,37.60108V601.75393A37.60108,37.60108,0,0,1,899.669,639.355Z" transform="translate(0.00006 -297.905)"/></svg>)svg"},
        {"隐藏目录导航.svg", R"svg(<svg id="图层_1" data-name="图层 1" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 1000.00012 404.19"><defs><style>.cls-1{fill:#fff;}</style></defs><title>隐藏目录导航</title><rect class="cls-1" x="250.19" y="62.72998" width="124.72009" height="278.72003"/><rect class="cls-1" x="437.64001" y="62.72998" width="124.72009" height="278.72003"/><rect class="cls-1" x="625.09003" y="62.72998" width="124.72009" height="278.72003"/><rect class="cls-1" x="812.54004" y="62.72998" width="124.72009" height="278.72003"/><path d="M-.00006,335.50608V664.49392A37.60108,37.60108,0,0,0,37.601,702.095H962.399a37.60108,37.60108,0,0,0,37.60108-37.60108V335.50608A37.60108,37.60108,0,0,0,962.399,297.905H37.601A37.60108,37.60108,0,0,0-.00006,335.50608Zm62.74,266.24785V398.23606A37.60108,37.60108,0,0,1,100.341,360.635h49.51793A37.60108,37.60108,0,0,1,187.46,398.23606V601.75393A37.60108,37.60108,0,0,1,149.85894,639.355H100.341A37.60108,37.60108,0,0,1,62.73993,601.75393Zm274.569,37.60108h-49.5178A37.60109,37.60109,0,0,1,250.19,601.75393V398.23606A37.60109,37.60109,0,0,1,287.79109,360.635h49.5178A37.60108,37.60108,0,0,1,374.91,398.23606V601.75393A37.60108,37.60108,0,0,1,337.30889,639.355Zm187.44995,0H475.241A37.60109,37.60109,0,0,1,437.64,601.75393V398.23606A37.60109,37.60109,0,0,1,475.241,360.635h49.5178a37.60108,37.60108,0,0,1,37.60108,37.60108V601.75393A37.60108,37.60108,0,0,1,524.75884,639.355Zm187.46,0H662.701a37.60109,37.60109,0,0,1-37.60109-37.60108V398.23606A37.60109,37.60109,0,0,1,662.701,360.635h49.5178a37.60109,37.60109,0,0,1,37.60109,37.60108V601.75393A37.60109,37.60109,0,0,1,712.2188,639.355Zm187.4502,0H850.151a37.60108,37.60108,0,0,1-37.60108-37.60108V398.23606A37.60108,37.60108,0,0,1,850.151,360.635H899.669a37.60108,37.60108,0,0,1,37.60108,37.60108V601.75393A37.60108,37.60108,0,0,1,899.669,639.355Z" transform="translate(0.00006 -297.905)"/></svg>)svg"},
        {"隐藏筛选器", R"svg(<svg id="图层_1" data-name="图层 1" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 1000.00012 404.19"><defs><style>.cls-1{fill:#fff;}</style></defs><title>隐藏筛选器</title><rect class="cls-1" x="62.73999" y="62.72998" width="124.72009" height="278.72003"/><rect class="cls-1" x="250.19" y="62.72998" width="124.72009" height="278.72003"/><rect class="cls-1" x="437.64001" y="62.72998" width="124.72009" height="278.72003"/><rect class="cls-1" x="625.09003" y="62.72998" width="124.72009" height="278.72003"/><path d="M.14061,335.26052V664.24835A37.60109,37.60109,0,0,0,37.7417,701.84944h924.798a37.60109,37.60109,0,0,0,37.60109-37.60109V335.26052a37.60109,37.60109,0,0,0-37.60109-37.60109H37.7417A37.60109,37.60109,0,0,0,.14061,335.26052Zm62.74,266.24784V397.9905a37.60109,37.60109,0,0,1,37.60109-37.60109h49.51792A37.60109,37.60109,0,0,1,187.6007,397.9905V601.50836a37.60109,37.60109,0,0,1-37.60109,37.60109H100.48169A37.60109,37.60109,0,0,1,62.8806,601.50836Zm274.569,37.60109h-49.5178a37.60109,37.60109,0,0,1-37.60108-37.60109V397.9905a37.60109,37.60109,0,0,1,37.60108-37.60109h49.5178a37.60109,37.60109,0,0,1,37.60109,37.60109V601.50836A37.60109,37.60109,0,0,1,337.44956,639.10945Zm187.44995,0h-49.5178a37.60109,37.60109,0,0,1-37.60108-37.60109V397.9905a37.60109,37.60109,0,0,1,37.60108-37.60109h49.5178A37.60109,37.60109,0,0,1,562.5006,397.9905V601.50836A37.60109,37.60109,0,0,1,524.89951,639.10945Zm187.46,0H662.84167a37.60109,37.60109,0,0,1-37.60108-37.60109V397.9905a37.60109,37.60109,0,0,1,37.60108-37.60109h49.51781a37.60109,37.60109,0,0,1,37.60108,37.60109V601.50836A37.60109,37.60109,0,0,1,712.35948,639.10945Zm187.45019,0h-49.518a37.60109,37.60109,0,0,1-37.60109-37.60109V397.9905a37.60109,37.60109,0,0,1,37.60109-37.60109h49.518a37.60109,37.60109,0,0,1,37.60109,37.60109V601.50836A37.60109,37.60109,0,0,1,899.80967,639.10945Z" transform="translate(-0.14061 -297.65943)"/></svg>)svg"},
        {"隐藏筛选器.svg", R"svg(<svg id="图层_1" data-name="图层 1" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 1000.00012 404.19"><defs><style>.cls-1{fill:#fff;}</style></defs><title>隐藏筛选器</title><rect class="cls-1" x="62.73999" y="62.72998" width="124.72009" height="278.72003"/><rect class="cls-1" x="250.19" y="62.72998" width="124.72009" height="278.72003"/><rect class="cls-1" x="437.64001" y="62.72998" width="124.72009" height="278.72003"/><rect class="cls-1" x="625.09003" y="62.72998" width="124.72009" height="278.72003"/><path d="M.14061,335.26052V664.24835A37.60109,37.60109,0,0,0,37.7417,701.84944h924.798a37.60109,37.60109,0,0,0,37.60109-37.60109V335.26052a37.60109,37.60109,0,0,0-37.60109-37.60109H37.7417A37.60109,37.60109,0,0,0,.14061,335.26052Zm62.74,266.24784V397.9905a37.60109,37.60109,0,0,1,37.60109-37.60109h49.51792A37.60109,37.60109,0,0,1,187.6007,397.9905V601.50836a37.60109,37.60109,0,0,1-37.60109,37.60109H100.48169A37.60109,37.60109,0,0,1,62.8806,601.50836Zm274.569,37.60109h-49.5178a37.60109,37.60109,0,0,1-37.60108-37.60109V397.9905a37.60109,37.60109,0,0,1,37.60108-37.60109h49.5178a37.60109,37.60109,0,0,1,37.60109,37.60109V601.50836A37.60109,37.60109,0,0,1,337.44956,639.10945Zm187.44995,0h-49.5178a37.60109,37.60109,0,0,1-37.60108-37.60109V397.9905a37.60109,37.60109,0,0,1,37.60108-37.60109h49.5178A37.60109,37.60109,0,0,1,562.5006,397.9905V601.50836A37.60109,37.60109,0,0,1,524.89951,639.10945Zm187.46,0H662.84167a37.60109,37.60109,0,0,1-37.60108-37.60109V397.9905a37.60109,37.60109,0,0,1,37.60108-37.60109h49.51781a37.60109,37.60109,0,0,1,37.60108,37.60109V601.50836A37.60109,37.60109,0,0,1,712.35948,639.10945Zm187.45019,0h-49.518a37.60109,37.60109,0,0,1-37.60109-37.60109V397.9905a37.60109,37.60109,0,0,1,37.60109-37.60109h49.518a37.60109,37.60109,0,0,1,37.60109,37.60109V601.50836A37.60109,37.60109,0,0,1,899.80967,639.10945Z" transform="translate(-0.14061 -297.65943)"/></svg>)svg"},
        {"seach-3", R"svg(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 867.06295 811.2703"><path fill="currentColor" d="M837.33852,813.21517l-151.61-151.61a340.49438,340.49438,0,0,0,64.96-200.59c0-188.94-153.17-342.11-342.11-342.11s-342.11,153.17-342.11,342.11,153.17,342.11,342.11,342.11a340.49475,340.49475,0,0,0,200.59-64.96l151.61,151.61a54.13609,54.13609,0,1,0,76.56-76.56Zm-229.6-229.6a233.75854,233.75854,0,0,1-199.16,111.24c-129.15,0-233.84-104.69-233.84-233.84a233.75862,233.75862,0,0,1,233.84-233.84c129.15,0,233.84,104.69,233.84,233.84A232.72707,232.72707,0,0,1,607.73849,583.61513Z" transform="translate(-66.46853 -94.36485)"/><polygon fill="currentColor" points="721.506 149.874 867.063 0 575.95 0 721.506 149.874"/></svg>)svg"},
        {"seach_3", R"svg(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 867.06295 811.2703"><path fill="currentColor" d="M837.33852,813.21517l-151.61-151.61a340.49438,340.49438,0,0,0,64.96-200.59c0-188.94-153.17-342.11-342.11-342.11s-342.11,153.17-342.11,342.11,153.17,342.11,342.11,342.11a340.49475,340.49475,0,0,0,200.59-64.96l151.61,151.61a54.13609,54.13609,0,1,0,76.56-76.56Zm-229.6-229.6a233.75854,233.75854,0,0,1-199.16,111.24c-129.15,0-233.84-104.69-233.84-233.84a233.75862,233.75862,0,0,1,233.84-233.84c129.15,0,233.84,104.69,233.84,233.84A232.72707,232.72707,0,0,1,607.73849,583.61513Z" transform="translate(-66.46853 -94.36485)"/><polygon fill="currentColor" points="721.506 149.874 867.063 0 575.95 0 721.506 149.874"/></svg>)svg"},
        {"seach-3.svg", R"svg(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 867.06295 811.2703"><path fill="currentColor" d="M837.33852,813.21517l-151.61-151.61a340.49438,340.49438,0,0,0,64.96-200.59c0-188.94-153.17-342.11-342.11-342.11s-342.11,153.17-342.11,342.11,153.17,342.11,342.11,342.11a340.49475,340.49475,0,0,0,200.59-64.96l151.61,151.61a54.13609,54.13609,0,1,0,76.56-76.56Zm-229.6-229.6a233.75854,233.75854,0,0,1-199.16,111.24c-129.15,0-233.84-104.69-233.84-233.84a233.75862,233.75862,0,0,1,233.84-233.84c129.15,0,233.84,104.69,233.84,233.84A232.72707,232.72707,0,0,1,607.73849,583.61513Z" transform="translate(-66.46853 -94.36485)"/><polygon fill="currentColor" points="721.506 149.874 867.063 0 575.95 0 721.506 149.874"/></svg>)svg"},
        {"search-3", R"svg(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 867.06295 811.2703"><path fill="currentColor" d="M837.33852,813.21517l-151.61-151.61a340.49438,340.49438,0,0,0,64.96-200.59c0-188.94-153.17-342.11-342.11-342.11s-342.11,153.17-342.11,342.11,153.17,342.11,342.11,342.11a340.49475,340.49475,0,0,0,200.59-64.96l151.61,151.61a54.13609,54.13609,0,1,0,76.56-76.56Zm-229.6-229.6a233.75854,233.75854,0,0,1-199.16,111.24c-129.15,0-233.84-104.69-233.84-233.84a233.75862,233.75862,0,0,1,233.84-233.84c129.15,0,233.84,104.69,233.84,233.84A232.72707,232.72707,0,0,1,607.73849,583.61513Z" transform="translate(-66.46853 -94.36485)"/><polygon fill="currentColor" points="721.506 149.874 867.063 0 575.95 0 721.506 149.874"/></svg>)svg"},
        {"search_3", R"svg(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 867.06295 811.2703"><path fill="currentColor" d="M837.33852,813.21517l-151.61-151.61a340.49438,340.49438,0,0,0,64.96-200.59c0-188.94-153.17-342.11-342.11-342.11s-342.11,153.17-342.11,342.11,153.17,342.11,342.11,342.11a340.49475,340.49475,0,0,0,200.59-64.96l151.61,151.61a54.13609,54.13609,0,1,0,76.56-76.56Zm-229.6-229.6a233.75854,233.75854,0,0,1-199.16,111.24c-129.15,0-233.84-104.69-233.84-233.84a233.75862,233.75862,0,0,1,233.84-233.84c129.15,0,233.84,104.69,233.84,233.84A232.72707,232.72707,0,0,1,607.73849,583.61513Z" transform="translate(-66.46853 -94.36485)"/><polygon fill="currentColor" points="721.506 149.874 867.063 0 575.95 0 721.506 149.874"/></svg>)svg"},
        {"search-3.svg", R"svg(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 867.06295 811.2703"><path fill="currentColor" d="M837.33852,813.21517l-151.61-151.61a340.49438,340.49438,0,0,0,64.96-200.59c0-188.94-153.17-342.11-342.11-342.11s-342.11,153.17-342.11,342.11,153.17,342.11,342.11,342.11a340.49475,340.49475,0,0,0,200.59-64.96l151.61,151.61a54.13609,54.13609,0,1,0,76.56-76.56Zm-229.6-229.6a233.75854,233.75854,0,0,1-199.16,111.24c-129.15,0-233.84-104.69-233.84-233.84a233.75862,233.75862,0,0,1,233.84-233.84c129.15,0,233.84,104.69,233.84,233.84A232.72707,232.72707,0,0,1,607.73849,583.61513Z" transform="translate(-66.46853 -94.36485)"/><polygon fill="currentColor" points="721.506 149.874 867.063 0 575.95 0 721.506 149.874"/></svg>)svg"},
        {"seach-7", R"svg(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 909.37866 848.83797"><path fill="currentColor" d="M751.749,839.72375l-138.938-138.938a312.03474,312.03474,0,0,0,59.53041-183.824c0-173.14781-140.36756-313.51538-313.51538-313.51538S45.31067,343.81389,45.31067,516.9617,185.67824,830.47708,358.82605,830.47708a312.03514,312.03514,0,0,0,183.82409-59.53046l138.938,138.938A49.61123,49.61123,0,1,0,751.749,839.72375ZM541.33962,629.31439A214.22028,214.22028,0,0,1,358.82605,731.2566c-118.35527,0-214.295-95.93969-214.295-214.2949a214.22039,214.22039,0,0,1,214.295-214.295c118.35527,0,214.295,95.93969,214.295,214.295A213.275,213.275,0,0,1,541.33962,629.31439Z" transform="translate(-45.31067 -75.58101)"/><polygon fill="currentColor" points="744.199 194.645 909.379 0 579.02 0 744.199 194.645"/></svg>)svg"},
        {"seach_7", R"svg(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 909.37866 848.83797"><path fill="currentColor" d="M751.749,839.72375l-138.938-138.938a312.03474,312.03474,0,0,0,59.53041-183.824c0-173.14781-140.36756-313.51538-313.51538-313.51538S45.31067,343.81389,45.31067,516.9617,185.67824,830.47708,358.82605,830.47708a312.03514,312.03514,0,0,0,183.82409-59.53046l138.938,138.938A49.61123,49.61123,0,1,0,751.749,839.72375ZM541.33962,629.31439A214.22028,214.22028,0,0,1,358.82605,731.2566c-118.35527,0-214.295-95.93969-214.295-214.2949a214.22039,214.22039,0,0,1,214.295-214.295c118.35527,0,214.295,95.93969,214.295,214.295A213.275,213.275,0,0,1,541.33962,629.31439Z" transform="translate(-45.31067 -75.58101)"/><polygon fill="currentColor" points="744.199 194.645 909.379 0 579.02 0 744.199 194.645"/></svg>)svg"},
        {"seach-7.svg", R"svg(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 909.37866 848.83797"><path fill="currentColor" d="M751.749,839.72375l-138.938-138.938a312.03474,312.03474,0,0,0,59.53041-183.824c0-173.14781-140.36756-313.51538-313.51538-313.51538S45.31067,343.81389,45.31067,516.9617,185.67824,830.47708,358.82605,830.47708a312.03514,312.03514,0,0,0,183.82409-59.53046l138.938,138.938A49.61123,49.61123,0,1,0,751.749,839.72375ZM541.33962,629.31439A214.22028,214.22028,0,0,1,358.82605,731.2566c-118.35527,0-214.295-95.93969-214.295-214.2949a214.22039,214.22039,0,0,1,214.295-214.295c118.35527,0,214.295,95.93969,214.295,214.295A213.275,213.275,0,0,1,541.33962,629.31439Z" transform="translate(-45.31067 -75.58101)"/><polygon fill="currentColor" points="744.199 194.645 909.379 0 579.02 0 744.199 194.645"/></svg>)svg"},
        {"search-7", R"svg(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 909.37866 848.83797"><path fill="currentColor" d="M751.749,839.72375l-138.938-138.938a312.03474,312.03474,0,0,0,59.53041-183.824c0-173.14781-140.36756-313.51538-313.51538-313.51538S45.31067,343.81389,45.31067,516.9617,185.67824,830.47708,358.82605,830.47708a312.03514,312.03514,0,0,0,183.82409-59.53046l138.938,138.938A49.61123,49.61123,0,1,0,751.749,839.72375ZM541.33962,629.31439A214.22028,214.22028,0,0,1,358.82605,731.2566c-118.35527,0-214.295-95.93969-214.295-214.2949a214.22039,214.22039,0,0,1,214.295-214.295c118.35527,0,214.295,95.93969,214.295,214.295A213.275,213.275,0,0,1,541.33962,629.31439Z" transform="translate(-45.31067 -75.58101)"/><polygon fill="currentColor" points="744.199 194.645 909.379 0 579.02 0 744.199 194.645"/></svg>)svg"},
        {"search_7", R"svg(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 909.37866 848.83797"><path fill="currentColor" d="M751.749,839.72375l-138.938-138.938a312.03474,312.03474,0,0,0,59.53041-183.824c0-173.14781-140.36756-313.51538-313.51538-313.51538S45.31067,343.81389,45.31067,516.9617,185.67824,830.47708,358.82605,830.47708a312.03514,312.03514,0,0,0,183.82409-59.53046l138.938,138.938A49.61123,49.61123,0,1,0,751.749,839.72375ZM541.33962,629.31439A214.22028,214.22028,0,0,1,358.82605,731.2566c-118.35527,0-214.295-95.93969-214.295-214.2949a214.22039,214.22039,0,0,1,214.295-214.295c118.35527,0,214.295,95.93969,214.295,214.295A213.275,213.275,0,0,1,541.33962,629.31439Z" transform="translate(-45.31067 -75.58101)"/><polygon fill="currentColor" points="744.199 194.645 909.379 0 579.02 0 744.199 194.645"/></svg>)svg"},
        {"search-7.svg", R"svg(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 909.37866 848.83797"><path fill="currentColor" d="M751.749,839.72375l-138.938-138.938a312.03474,312.03474,0,0,0,59.53041-183.824c0-173.14781-140.36756-313.51538-313.51538-313.51538S45.31067,343.81389,45.31067,516.9617,185.67824,830.47708,358.82605,830.47708a312.03514,312.03514,0,0,0,183.82409-59.53046l138.938,138.938A49.61123,49.61123,0,1,0,751.749,839.72375ZM541.33962,629.31439A214.22028,214.22028,0,0,1,358.82605,731.2566c-118.35527,0-214.295-95.93969-214.295-214.2949a214.22039,214.22039,0,0,1,214.295-214.295c118.35527,0,214.295,95.93969,214.295,214.295A213.275,213.275,0,0,1,541.33962,629.31439Z" transform="translate(-45.31067 -75.58101)"/><polygon fill="currentColor" points="744.199 194.645 909.379 0 579.02 0 744.199 194.645"/></svg>)svg"},
        {"seach-8", R"svg(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 909.37866 848.83797"><defs><style>.cls-1{fill:#fff;}</style></defs><path class="cls-1" d="M751.749,839.72375l-138.938-138.938a312.03474,312.03474,0,0,0,59.53041-183.824c0-173.14781-140.36756-313.51538-313.51538-313.51538S45.31067,343.81389,45.31067,516.9617,185.67824,830.47708,358.82605,830.47708a312.03514,312.03514,0,0,0,183.82409-59.53046l138.938,138.938A49.61123,49.61123,0,1,0,751.749,839.72375ZM541.33962,629.31439A214.22028,214.22028,0,0,1,358.82605,731.2566c-118.35527,0-214.295-95.93969-214.295-214.2949a214.22039,214.22039,0,0,1,214.295-214.295c118.35527,0,214.295,95.93969,214.295,214.295A213.275,213.275,0,0,1,541.33962,629.31439Z" transform="translate(-45.31067 -75.58101)"/><polygon class="cls-1" points="744.199 194.645 909.379 0 579.02 0 744.199 194.645"/></svg>)svg"},
        {"seach_8", R"svg(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 909.37866 848.83797"><defs><style>.cls-1{fill:#fff;}</style></defs><path class="cls-1" d="M751.749,839.72375l-138.938-138.938a312.03474,312.03474,0,0,0,59.53041-183.824c0-173.14781-140.36756-313.51538-313.51538-313.51538S45.31067,343.81389,45.31067,516.9617,185.67824,830.47708,358.82605,830.47708a312.03514,312.03514,0,0,0,183.82409-59.53046l138.938,138.938A49.61123,49.61123,0,1,0,751.749,839.72375ZM541.33962,629.31439A214.22028,214.22028,0,0,1,358.82605,731.2566c-118.35527,0-214.295-95.93969-214.295-214.2949a214.22039,214.22039,0,0,1,214.295-214.295c118.35527,0,214.295,95.93969,214.295,214.295A213.275,213.275,0,0,1,541.33962,629.31439Z" transform="translate(-45.31067 -75.58101)"/><polygon class="cls-1" points="744.199 194.645 909.379 0 579.02 0 744.199 194.645"/></svg>)svg"},
        {"seach-8.svg", R"svg(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 909.37866 848.83797"><defs><style>.cls-1{fill:#fff;}</style></defs><path class="cls-1" d="M751.749,839.72375l-138.938-138.938a312.03474,312.03474,0,0,0,59.53041-183.824c0-173.14781-140.36756-313.51538-313.51538-313.51538S45.31067,343.81389,45.31067,516.9617,185.67824,830.47708,358.82605,830.47708a312.03514,312.03514,0,0,0,183.82409-59.53046l138.938,138.938A49.61123,49.61123,0,1,0,751.749,839.72375ZM541.33962,629.31439A214.22028,214.22028,0,0,1,358.82605,731.2566c-118.35527,0-214.295-95.93969-214.295-214.2949a214.22039,214.22039,0,0,1,214.295-214.295c118.35527,0,214.295,95.93969,214.295,214.295A213.275,213.275,0,0,1,541.33962,629.31439Z" transform="translate(-45.31067 -75.58101)"/><polygon class="cls-1" points="744.199 194.645 909.379 0 579.02 0 744.199 194.645"/></svg>)svg"},
        {"search-8", R"svg(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 909.37866 848.83797"><defs><style>.cls-1{fill:#fff;}</style></defs><path class="cls-1" d="M751.749,839.72375l-138.938-138.938a312.03474,312.03474,0,0,0,59.53041-183.824c0-173.14781-140.36756-313.51538-313.51538-313.51538S45.31067,343.81389,45.31067,516.9617,185.67824,830.47708,358.82605,830.47708a312.03514,312.03514,0,0,0,183.82409-59.53046l138.938,138.938A49.61123,49.61123,0,1,0,751.749,839.72375ZM541.33962,629.31439A214.22028,214.22028,0,0,1,358.82605,731.2566c-118.35527,0-214.295-95.93969-214.295-214.2949a214.22039,214.22039,0,0,1,214.295-214.295c118.35527,0,214.295,95.93969,214.295,214.295A213.275,213.275,0,0,1,541.33962,629.31439Z" transform="translate(-45.31067 -75.58101)"/><polygon class="cls-1" points="744.199 194.645 909.379 0 579.02 0 744.199 194.645"/></svg>)svg"},
        {"search_8", R"svg(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 909.37866 848.83797"><defs><style>.cls-1{fill:#fff;}</style></defs><path class="cls-1" d="M751.749,839.72375l-138.938-138.938a312.03474,312.03474,0,0,0,59.53041-183.824c0-173.14781-140.36756-313.51538-313.51538-313.51538S45.31067,343.81389,45.31067,516.9617,185.67824,830.47708,358.82605,830.47708a312.03514,312.03514,0,0,0,183.82409-59.53046l138.938,138.938A49.61123,49.61123,0,1,0,751.749,839.72375ZM541.33962,629.31439A214.22028,214.22028,0,0,1,358.82605,731.2566c-118.35527,0-214.295-95.93969-214.295-214.2949a214.22039,214.22039,0,0,1,214.295-214.295c118.35527,0,214.295,95.93969,214.295,214.295A213.275,213.275,0,0,1,541.33962,629.31439Z" transform="translate(-45.31067 -75.58101)"/><polygon class="cls-1" points="744.199 194.645 909.379 0 579.02 0 744.199 194.645"/></svg>)svg"},
        {"search-8.svg", R"svg(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 909.37866 848.83797"><defs><style>.cls-1{fill:#fff;}</style></defs><path class="cls-1" d="M751.749,839.72375l-138.938-138.938a312.03474,312.03474,0,0,0,59.53041-183.824c0-173.14781-140.36756-313.51538-313.51538-313.51538S45.31067,343.81389,45.31067,516.9617,185.67824,830.47708,358.82605,830.47708a312.03514,312.03514,0,0,0,183.82409-59.53046l138.938,138.938A49.61123,49.61123,0,1,0,751.749,839.72375ZM541.33962,629.31439A214.22028,214.22028,0,0,1,358.82605,731.2566c-118.35527,0-214.295-95.93969-214.295-214.2949a214.22039,214.22039,0,0,1,214.295-214.295c118.35527,0,214.295,95.93969,214.295,214.295A213.275,213.275,0,0,1,541.33962,629.31439Z" transform="translate(-45.31067 -75.58101)"/><polygon class="cls-1" points="744.199 194.645 909.379 0 579.02 0 744.199 194.645"/></svg>)svg"},
        {"file_eps", R"svg(<?xml version="1.0" encoding="utf-8"?>
<svg version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
	 viewBox="0 0 800 800" style="enable-background:new 0 0 800 800;" xml:space="preserve" fill="currentColor">
<style type="text/css">
	.st0{fill-rule:evenodd;clip-rule:evenodd;}
	.st1{fill:none;}
</style>
<title>file_eps [#1727]</title>
<desc>Created with Sketch.</desc>
<g>
	<polygon class="st0" points="559.2,0 0,0 0,400 80,400 80,80 480,80 480,320 720,320 720,400 800,400 800,257.5 	"/>
	<polygon points="93,532.9 93,603.5 242,603.5 242,675.4 93,675.4 93,737.5 242,737.5 242,800.9 0,800.9 0,478 242,478 242,532.9 	
		"/>
	<g>
		<polygon class="st1" points="446.5,478 403.6,561.5 360.7,478 		"/>
		<polygon class="st1" points="446.6,800.9 360.7,800.9 403.6,717.3 		"/>
		<polygon points="443.6,639.4 526,799.7 525.4,800.9 446.6,800.9 403.6,717.3 360.7,800.9 281.9,800.9 281.2,799.7 363.6,639.4 
			281.2,479.1 281.8,478 360.7,478 403.6,561.5 446.5,478 525.4,478 526,479.1 		"/>
	</g>
	<polygon points="709,478.5 654,478.5 563,478.5 563,546.5 645.5,546.5 645.5,800.5 717.5,800.5 717.5,546.5 800,546.5 800,478.5 	
		"/>
</g>
</svg>)svg"},
        {"Ext.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<svg version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
	 viewBox="0 0 800 800" style="enable-background:new 0 0 800 800;" xml:space="preserve" fill="currentColor">
<style type="text/css">
	.st0{fill-rule:evenodd;clip-rule:evenodd;}
	.st1{fill:none;}
</style>
<title>file_eps [#1727]</title>
<desc>Created with Sketch.</desc>
<g>
	<polygon class="st0" points="559.2,0 0,0 0,400 80,400 80,80 480,80 480,320 720,320 720,400 800,400 800,257.5 	"/>
	<polygon points="93,532.9 93,603.5 242,603.5 242,675.4 93,675.4 93,737.5 242,737.5 242,800.9 0,800.9 0,478 242,478 242,532.9 	
		"/>
	<g>
		<polygon class="st1" points="446.5,478 403.6,561.5 360.7,478 		"/>
		<polygon class="st1" points="446.6,800.9 360.7,800.9 403.6,717.3 		"/>
		<polygon points="443.6,639.4 526,799.7 525.4,800.9 446.6,800.9 403.6,717.3 360.7,800.9 281.9,800.9 281.2,799.7 363.6,639.4 
			281.2,479.1 281.8,478 360.7,478 403.6,561.5 446.5,478 525.4,478 526,479.1 		"/>
	</g>
	<polygon points="709,478.5 654,478.5 563,478.5 563,546.5 645.5,546.5 645.5,800.5 717.5,800.5 717.5,546.5 800,546.5 800,478.5 	
		"/>
</g>
</svg>)svg"},
        {"Ext", R"svg(<?xml version="1.0" encoding="utf-8"?>
<svg version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
	 viewBox="0 0 800 800" style="enable-background:new 0 0 800 800;" xml:space="preserve" fill="currentColor">
<style type="text/css">
	.st0{fill-rule:evenodd;clip-rule:evenodd;}
	.st1{fill:none;}
</style>
<title>file_eps [#1727]</title>
<desc>Created with Sketch.</desc>
<g>
	<polygon class="st0" points="559.2,0 0,0 0,400 80,400 80,80 480,80 480,320 720,320 720,400 800,400 800,257.5 	"/>
	<polygon points="93,532.9 93,603.5 242,603.5 242,675.4 93,675.4 93,737.5 242,737.5 242,800.9 0,800.9 0,478 242,478 242,532.9 	
		"/>
	<g>
		<polygon class="st1" points="446.5,478 403.6,561.5 360.7,478 		"/>
		<polygon class="st1" points="446.6,800.9 360.7,800.9 403.6,717.3 		"/>
		<polygon points="443.6,639.4 526,799.7 525.4,800.9 446.6,800.9 403.6,717.3 360.7,800.9 281.9,800.9 281.2,799.7 363.6,639.4 
			281.2,479.1 281.8,478 360.7,478 403.6,561.5 446.5,478 525.4,478 526,479.1 		"/>
	</g>
	<polygon points="709,478.5 654,478.5 563,478.5 563,546.5 645.5,546.5 645.5,800.5 717.5,800.5 717.5,546.5 800,546.5 800,478.5 	
		"/>
</g>
</svg>)svg"},
        {"ext", R"svg(<?xml version="1.0" encoding="utf-8"?>
<svg version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
	 viewBox="0 0 800 800" style="enable-background:new 0 0 800 800;" xml:space="preserve" fill="currentColor">
<style type="text/css">
	.st0{fill-rule:evenodd;clip-rule:evenodd;}
	.st1{fill:none;}
</style>
<title>file_eps [#1727]</title>
<desc>Created with Sketch.</desc>
<g>
	<polygon class="st0" points="559.2,0 0,0 0,400 80,400 80,80 480,80 480,320 720,320 720,400 800,400 800,257.5 	"/>
	<polygon points="93,532.9 93,603.5 242,603.5 242,675.4 93,675.4 93,737.5 242,737.5 242,800.9 0,800.9 0,478 242,478 242,532.9 	
		"/>
	<g>
		<polygon class="st1" points="446.5,478 403.6,561.5 360.7,478 		"/>
		<polygon class="st1" points="446.6,800.9 360.7,800.9 403.6,717.3 		"/>
		<polygon points="443.6,639.4 526,799.7 525.4,800.9 446.6,800.9 403.6,717.3 360.7,800.9 281.9,800.9 281.2,799.7 363.6,639.4 
			281.2,479.1 281.8,478 360.7,478 403.6,561.5 446.5,478 525.4,478 526,479.1 		"/>
	</g>
	<polygon points="709,478.5 654,478.5 563,478.5 563,546.5 645.5,546.5 645.5,800.5 717.5,800.5 717.5,546.5 800,546.5 800,478.5 	
		"/>
</g>
</svg>)svg"},
        {"uncategorized-2", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Generator: Adobe Illustrator 23.0.0, SVG Export Plug-In . SVG Version: 6.00 Build 0)  -->
<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
	 viewBox="0 0 999 871.6" style="enable-background:new 0 0 999 871.6;" xml:space="preserve">
<title>未分类-1</title>
<path d="M999,264.7v109.7H890.6v-51.5c0-19.8-16-35.8-35.8-35.8h-132L531.3,720.6h104.4V829H86.1C38.5,829,0,790.5,0,743l0,0V128.6
	c0-47.5,38.5-86,86-86l0,0h328.1c16.4,0,32.5,6.5,42.8,19.2l0.1,0.1l93.4,115.5l1.1,1.4H913C960.5,178.7,999,217.2,999,264.7
	L999,264.7z"/>
<path d="M551.5,178.4v0.3l-1.1-1.4C550.8,177.7,551.2,178.1,551.5,178.4z"/>
<path d="M964.8,484.8c-9.8-30.7-27.9-49.2-32.3-53.5c-21.7-20.8-45.1-27.5-58.8-31.3c-20.2-5.5-46.7-5.4-46.7-5.4
	s-24.7-0.8-47.3,3.5c-42.7,8.1-83.9,45.2-99.5,86.7c-8.1,14.8-9.3,36.7-10.4,53.6c0,0,0,0,0,0c-0.1,1.2,0.9,2.1,2,2.1h75.9
	c0.9,0,1.7-0.6,2-1.6c0,0,0-0.1,0-0.1c3.9-15.1,9.9-29.1,20.9-40.4c22.9-23.5,63.8-23.1,85.6-9.9c25.1,15.1,32.2,44.4,15.9,67.2
	c-4.7,6.5-11.2,11.8-17.2,17.3c-16.1,14.7-32.8,28.7-48.5,43.8c-12,11.6-21.1,25.6-25.8,42.2c-5.3,19.1-5.6,38.6-5.2,58.3
	c0.1,5.7,1.5,7.1,7.2,7.1c10.6,0,21.2,0,31.9,0c10.9,0,21.8,0,32.7,0c4.9,0,6.4-1.4,6.6-6.4c0.2-6,0-12,0.5-17.9
	c0.7-9.9,1.3-20,7.8-28c5.1-6.4,10.6-12.5,16.6-17.9c20.3-18.5,41.7-35.7,59.7-56.7C964.8,566.5,979.9,532.4,964.8,484.8z"/>
<path d="M854.1,742c-24-0.1-48.1-0.1-72.1,0c-5.2,0-6.7,1.6-6.7,6.6c0,24.4,0,48.8,0,73.1c0,5.4,1.5,6.8,7,6.8c11.9,0,23.9,0,35.8,0
	c11.9,0,23.9,0,35.8,0c5.2,0,6.7-1.5,6.7-6.6c0.1-24.4,0.1-48.8,0-73.1C860.6,743.6,859.2,742,854.1,742z"/>
</svg>)svg"},
        {"Uncategorized-2.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Generator: Adobe Illustrator 23.0.0, SVG Export Plug-In . SVG Version: 6.00 Build 0)  -->
<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
	 viewBox="0 0 999 871.6" style="enable-background:new 0 0 999 871.6;" xml:space="preserve">
<title>未分类-1</title>
<path d="M999,264.7v109.7H890.6v-51.5c0-19.8-16-35.8-35.8-35.8h-132L531.3,720.6h104.4V829H86.1C38.5,829,0,790.5,0,743l0,0V128.6
	c0-47.5,38.5-86,86-86l0,0h328.1c16.4,0,32.5,6.5,42.8,19.2l0.1,0.1l93.4,115.5l1.1,1.4H913C960.5,178.7,999,217.2,999,264.7
	L999,264.7z"/>
<path d="M551.5,178.4v0.3l-1.1-1.4C550.8,177.7,551.2,178.1,551.5,178.4z"/>
<path d="M964.8,484.8c-9.8-30.7-27.9-49.2-32.3-53.5c-21.7-20.8-45.1-27.5-58.8-31.3c-20.2-5.5-46.7-5.4-46.7-5.4
	s-24.7-0.8-47.3,3.5c-42.7,8.1-83.9,45.2-99.5,86.7c-8.1,14.8-9.3,36.7-10.4,53.6c0,0,0,0,0,0c-0.1,1.2,0.9,2.1,2,2.1h75.9
	c0.9,0,1.7-0.6,2-1.6c0,0,0-0.1,0-0.1c3.9-15.1,9.9-29.1,20.9-40.4c22.9-23.5,63.8-23.1,85.6-9.9c25.1,15.1,32.2,44.4,15.9,67.2
	c-4.7,6.5-11.2,11.8-17.2,17.3c-16.1,14.7-32.8,28.7-48.5,43.8c-12,11.6-21.1,25.6-25.8,42.2c-5.3,19.1-5.6,38.6-5.2,58.3
	c0.1,5.7,1.5,7.1,7.2,7.1c10.6,0,21.2,0,31.9,0c10.9,0,21.8,0,32.7,0c4.9,0,6.4-1.4,6.6-6.4c0.2-6,0-12,0.5-17.9
	c0.7-9.9,1.3-20,7.8-28c5.1-6.4,10.6-12.5,16.6-17.9c20.3-18.5,41.7-35.7,59.7-56.7C964.8,566.5,979.9,532.4,964.8,484.8z"/>
<path d="M854.1,742c-24-0.1-48.1-0.1-72.1,0c-5.2,0-6.7,1.6-6.7,6.6c0,24.4,0,48.8,0,73.1c0,5.4,1.5,6.8,7,6.8c11.9,0,23.9,0,35.8,0
	c11.9,0,23.9,0,35.8,0c5.2,0,6.7-1.5,6.7-6.6c0.1-24.4,0.1-48.8,0-73.1C860.6,743.6,859.2,742,854.1,742z"/>
</svg>)svg"},
        {"Uncategorized-2", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Generator: Adobe Illustrator 23.0.0, SVG Export Plug-In . SVG Version: 6.00 Build 0)  -->
<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
	 viewBox="0 0 999 871.6" style="enable-background:new 0 0 999 871.6;" xml:space="preserve">
<title>未分类-1</title>
<path d="M999,264.7v109.7H890.6v-51.5c0-19.8-16-35.8-35.8-35.8h-132L531.3,720.6h104.4V829H86.1C38.5,829,0,790.5,0,743l0,0V128.6
	c0-47.5,38.5-86,86-86l0,0h328.1c16.4,0,32.5,6.5,42.8,19.2l0.1,0.1l93.4,115.5l1.1,1.4H913C960.5,178.7,999,217.2,999,264.7
	L999,264.7z"/>
<path d="M551.5,178.4v0.3l-1.1-1.4C550.8,177.7,551.2,178.1,551.5,178.4z"/>
<path d="M964.8,484.8c-9.8-30.7-27.9-49.2-32.3-53.5c-21.7-20.8-45.1-27.5-58.8-31.3c-20.2-5.5-46.7-5.4-46.7-5.4
	s-24.7-0.8-47.3,3.5c-42.7,8.1-83.9,45.2-99.5,86.7c-8.1,14.8-9.3,36.7-10.4,53.6c0,0,0,0,0,0c-0.1,1.2,0.9,2.1,2,2.1h75.9
	c0.9,0,1.7-0.6,2-1.6c0,0,0-0.1,0-0.1c3.9-15.1,9.9-29.1,20.9-40.4c22.9-23.5,63.8-23.1,85.6-9.9c25.1,15.1,32.2,44.4,15.9,67.2
	c-4.7,6.5-11.2,11.8-17.2,17.3c-16.1,14.7-32.8,28.7-48.5,43.8c-12,11.6-21.1,25.6-25.8,42.2c-5.3,19.1-5.6,38.6-5.2,58.3
	c0.1,5.7,1.5,7.1,7.2,7.1c10.6,0,21.2,0,31.9,0c10.9,0,21.8,0,32.7,0c4.9,0,6.4-1.4,6.6-6.4c0.2-6,0-12,0.5-17.9
	c0.7-9.9,1.3-20,7.8-28c5.1-6.4,10.6-12.5,16.6-17.9c20.3-18.5,41.7-35.7,59.7-56.7C964.8,566.5,979.9,532.4,964.8,484.8z"/>
<path d="M854.1,742c-24-0.1-48.1-0.1-72.1,0c-5.2,0-6.7,1.6-6.7,6.6c0,24.4,0,48.8,0,73.1c0,5.4,1.5,6.8,7,6.8c11.9,0,23.9,0,35.8,0
	c11.9,0,23.9,0,35.8,0c5.2,0,6.7-1.5,6.7-6.6c0.1-24.4,0.1-48.8,0-73.1C860.6,743.6,859.2,742,854.1,742z"/>
</svg>)svg"},
        {"uncategorized-1", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Generator: Adobe Illustrator 23.0.0, SVG Export Plug-In . SVG Version: 6.00 Build 0)  -->
<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
	 viewBox="0 0 999 871.6" style="enable-background:new 0 0 999 871.6;" xml:space="preserve">
<title>未分类-1</title>
<g>
	<path d="M999,264.7v279.8h-80.1V294.6c0-19.8-16-35.8-35.8-35.8H722.8L531.3,748.9h104.4V829H86.1C38.5,829,0,790.5,0,743
		c0,0,0,0,0,0V128.6c0-47.5,38.5-86,86-86c0,0,0,0,0,0h328.1c16.4,0,32.5,6.5,42.8,19.2l0.1,0.1l93.4,115.5l1.1,1.4H913
		C960.5,178.7,999,217.2,999,264.7C999,264.7,999,264.7,999,264.7z"/>
	<path d="M551.5,178.4l0,0.3l-1.1-1.4C550.8,177.7,551.2,178.1,551.5,178.4z"/>
	<path d="M727.8,722.6c-3.6-9.7-5.4-20-5.4-30.4c0-25.5,12.1-48.5,36.2-68.9c15-12.7,25.1-23.3,30.4-31.7c5.2-8.2,8-17.7,7.9-27.4
		c0-10.7-3.8-19.5-11.3-26.4c-7.6-6.9-17.3-10.3-29.2-10.3c-27.9,0-53.9,10.7-77.9,32.2v-70.2c25.2-15.1,53.9-22.7,86.1-22.7
		c32.6,0,58.4,7.6,77.3,22.7c18.9,15.1,28.3,36.6,28.3,64.6c0,16.8-3.7,31.9-11.1,45.5c-7.4,13.6-21,28.5-40.8,44.6
		c-15.9,13.3-25.7,23.6-29.5,31c-3.7,7.3-5.7,15.4-5.7,23.6c0,8.3,2.1,16.5,6.1,23.8L727.8,722.6z M718.5,790.1
		c-0.2-10.7,4.5-20.9,12.7-27.8c8.5-7.3,18.7-11,30.8-11c11.6,0,21.6,3.7,30,11c8.2,6.9,12.8,17.1,12.6,27.8
		c0.2,10.5-4.1,20.6-11.8,27.8c-7.6,7.4-17.8,11.4-28.3,11.2c-14,0-25.2-3.9-33.5-11.6C722.6,809.8,718.5,800.6,718.5,790.1z"/>
</g>
</svg>
)svg"},
        {"save-Thumbnail.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Generator: Adobe Illustrator 23.0.0, SVG Export Plug-In . SVG Version: 6.00 Build 0)  -->
<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
	 viewBox="0 0 1000 1000" style="enable-background:new 0 0 1000 1000;" xml:space="preserve">
<style type="text/css">
	.st0{fill-rule:evenodd;clip-rule:evenodd;}
</style>
<title>save-Thumbnail.svg</title>
<g>
	<path d="M999.5,286.4c0-47.5-38.5-86-86-86l0,0H552v-0.3c-42.5-41.8-71.3-90.4-94.6-116.7c-10.9-12.2-26.4-19.2-42.8-19.2H86.5
		c-47.5,0-86,38.5-86,86l0,0v699.5c0,47.5,38.5,86,86,86l0,0h607.7v-78.6c-81.1,15.4-165-3.7-231.5-52.6
		c-86.2-61.5-131.9-165.2-119-270.3c15.7-149,141.8-265.2,295-265.2c163.9,0.1,296.8,133.1,296.6,297c0,32.7-5.4,65.1-16,96v0.4
		h80.1L999.5,286.4L999.5,286.4z"/>
	<path class="st0" d="M814.3,570.7c0-10.1-0.9-20.3-2.8-30.6l50.6-43.8l-48-82l-63.8,21.3c-16.1-13.4-34.5-23.9-53.9-30.9
		l-13.2-64.9h-95.7l-13.2,64.9c-19.6,7-37.6,17.4-53.9,30.9l-63.8-21.3l-48,82l50.6,43.8c-1.9,10.3-2.8,20.5-2.8,30.6
		s0.9,20.3,2.8,30.6l-50.6,43.8l48,82l63.8-21.3c16.1,13.4,34.5,23.9,53.9,30.9l13.2,64.9h95.7l13.2-64.9
		c19.6-7,37.6-17.4,53.9-30.9l63.8,21.3l48-82l-50.6-43.8C813.3,591,814.3,580.8,814.3,570.7 M635.3,665.8
		c-52.5,0-95.1-42.7-95.1-95.1s42.7-95.1,95.1-95.1c52.5,0,95.1,42.7,95.1,95.1S687.8,665.8,635.3,665.8"/>
</g>
</svg>
)svg"},
        {"save-Thumbnail", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Generator: Adobe Illustrator 23.0.0, SVG Export Plug-In . SVG Version: 6.00 Build 0)  -->
<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
	 viewBox="0 0 1000 1000" style="enable-background:new 0 0 1000 1000;" xml:space="preserve">
<style type="text/css">
	.st0{fill-rule:evenodd;clip-rule:evenodd;}
</style>
<title>save-Thumbnail.svg</title>
<g>
	<path d="M999.5,286.4c0-47.5-38.5-86-86-86l0,0H552v-0.3c-42.5-41.8-71.3-90.4-94.6-116.7c-10.9-12.2-26.4-19.2-42.8-19.2H86.5
		c-47.5,0-86,38.5-86,86l0,0v699.5c0,47.5,38.5,86,86,86l0,0h607.7v-78.6c-81.1,15.4-165-3.7-231.5-52.6
		c-86.2-61.5-131.9-165.2-119-270.3c15.7-149,141.8-265.2,295-265.2c163.9,0.1,296.8,133.1,296.6,297c0,32.7-5.4,65.1-16,96v0.4
		h80.1L999.5,286.4L999.5,286.4z"/>
	<path class="st0" d="M814.3,570.7c0-10.1-0.9-20.3-2.8-30.6l50.6-43.8l-48-82l-63.8,21.3c-16.1-13.4-34.5-23.9-53.9-30.9
		l-13.2-64.9h-95.7l-13.2,64.9c-19.6,7-37.6,17.4-53.9,30.9l-63.8-21.3l-48,82l50.6,43.8c-1.9,10.3-2.8,20.5-2.8,30.6
		s0.9,20.3,2.8,30.6l-50.6,43.8l48,82l63.8-21.3c16.1,13.4,34.5,23.9,53.9,30.9l13.2,64.9h95.7l13.2-64.9
		c19.6-7,37.6-17.4,53.9-30.9l63.8,21.3l48-82l-50.6-43.8C813.3,591,814.3,580.8,814.3,570.7 M635.3,665.8
		c-52.5,0-95.1-42.7-95.1-95.1s42.7-95.1,95.1-95.1c52.5,0,95.1,42.7,95.1,95.1S687.8,665.8,635.3,665.8"/>
</g>
</svg>
)svg"},
        {"save_Thumbnail", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Generator: Adobe Illustrator 23.0.0, SVG Export Plug-In . SVG Version: 6.00 Build 0)  -->
<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
	 viewBox="0 0 1000 1000" style="enable-background:new 0 0 1000 1000;" xml:space="preserve">
<style type="text/css">
	.st0{fill-rule:evenodd;clip-rule:evenodd;}
</style>
<title>save-Thumbnail.svg</title>
<g>
	<path d="M999.5,286.4c0-47.5-38.5-86-86-86l0,0H552v-0.3c-42.5-41.8-71.3-90.4-94.6-116.7c-10.9-12.2-26.4-19.2-42.8-19.2H86.5
		c-47.5,0-86,38.5-86,86l0,0v699.5c0,47.5,38.5,86,86,86l0,0h607.7v-78.6c-81.1,15.4-165-3.7-231.5-52.6
		c-86.2-61.5-131.9-165.2-119-270.3c15.7-149,141.8-265.2,295-265.2c163.9,0.1,296.8,133.1,296.6,297c0,32.7-5.4,65.1-16,96v0.4
		h80.1L999.5,286.4L999.5,286.4z"/>
	<path class="st0" d="M814.3,570.7c0-10.1-0.9-20.3-2.8-30.6l50.6-43.8l-48-82l-63.8,21.3c-16.1-13.4-34.5-23.9-53.9-30.9
		l-13.2-64.9h-95.7l-13.2,64.9c-19.6,7-37.6,17.4-53.9,30.9l-63.8-21.3l-48,82l50.6,43.8c-1.9,10.3-2.8,20.5-2.8,30.6
		s0.9,20.3,2.8,30.6l-50.6,43.8l48,82l63.8-21.3c16.1,13.4,34.5,23.9,53.9,30.9l13.2,64.9h95.7l13.2-64.9
		c19.6-7,37.6-17.4,53.9-30.9l63.8,21.3l48-82l-50.6-43.8C813.3,591,814.3,580.8,814.3,570.7 M635.3,665.8
		c-52.5,0-95.1-42.7-95.1-95.1s42.7-95.1,95.1-95.1c52.5,0,95.1,42.7,95.1,95.1S687.8,665.8,635.3,665.8"/>
</g>
</svg>
)svg"},
        {"Folder-Seach-2.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Generator: Adobe Illustrator 23.0.0, SVG Export Plug-In . SVG Version: 6.00 Build 0)  -->
<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
	 viewBox="0 0 1000 1000" style="enable-background:new 0 0 1000 1000;" xml:space="preserve">
<style type="text/css">
	.st0{fill-rule:evenodd;clip-rule:evenodd;}
</style>
<title>在当前文件夹里查找-2</title>
<g>
	<path d="M999.5,286.4c0-47.5-38.5-86-86-86l0,0H552v-0.3c-42.5-41.8-71.3-90.4-94.6-116.7c-10.9-12.2-26.4-19.2-42.8-19.2H86.5
		c-47.5,0-86,38.5-86,86l0,0v699.5c0,47.5,38.5,86,86,86l0,0h607.7v-78.6c-81.1,15.4-165-3.7-231.5-52.6
		c-86.2-61.5-131.9-165.2-119-270.3c15.7-149,141.8-265.2,295-265.2c163.9,0.1,296.8,133.1,296.6,297c0,32.7-5.4,65.1-16,96v0.4
		h80.1L999.5,286.4L999.5,286.4z"/>
	<path class="st0" d="M814.3,570.7c0-10.1-0.9-20.3-2.8-30.6l50.6-43.8l-48-82l-63.8,21.3c-16.1-13.4-34.5-23.9-53.9-30.9
		l-13.2-64.9h-95.7l-13.2,64.9c-19.6,7-37.6,17.4-53.9,30.9l-63.8-21.3l-48,82l50.6,43.8c-1.9,10.3-2.8,20.5-2.8,30.6
		s0.9,20.3,2.8,30.6l-50.6,43.8l48,82l63.8-21.3c16.1,13.4,34.5,23.9,53.9,30.9l13.2,64.9h95.7l13.2-64.9
		c19.6-7,37.6-17.4,53.9-30.9l63.8,21.3l48-82l-50.6-43.8C813.3,591,814.3,580.8,814.3,570.7 M635.3,665.8
		c-52.5,0-95.1-42.7-95.1-95.1s42.7-95.1,95.1-95.1c52.5,0,95.1,42.7,95.1,95.1S687.8,665.8,635.3,665.8"/>
</g>
</svg>
)svg"},
        {"Folder-Seach-2", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Generator: Adobe Illustrator 23.0.0, SVG Export Plug-In . SVG Version: 6.00 Build 0)  -->
<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
	 viewBox="0 0 1000 1000" style="enable-background:new 0 0 1000 1000;" xml:space="preserve">
<style type="text/css">
	.st0{fill-rule:evenodd;clip-rule:evenodd;}
</style>
<title>在当前文件夹里查找-2</title>
<g>
	<path d="M999.5,286.4c0-47.5-38.5-86-86-86l0,0H552v-0.3c-42.5-41.8-71.3-90.4-94.6-116.7c-10.9-12.2-26.4-19.2-42.8-19.2H86.5
		c-47.5,0-86,38.5-86,86l0,0v699.5c0,47.5,38.5,86,86,86l0,0h607.7v-78.6c-81.1,15.4-165-3.7-231.5-52.6
		c-86.2-61.5-131.9-165.2-119-270.3c15.7-149,141.8-265.2,295-265.2c163.9,0.1,296.8,133.1,296.6,297c0,32.7-5.4,65.1-16,96v0.4
		h80.1L999.5,286.4L999.5,286.4z"/>
	<path class="st0" d="M814.3,570.7c0-10.1-0.9-20.3-2.8-30.6l50.6-43.8l-48-82l-63.8,21.3c-16.1-13.4-34.5-23.9-53.9-30.9
		l-13.2-64.9h-95.7l-13.2,64.9c-19.6,7-37.6,17.4-53.9,30.9l-63.8-21.3l-48,82l50.6,43.8c-1.9,10.3-2.8,20.5-2.8,30.6
		s0.9,20.3,2.8,30.6l-50.6,43.8l48,82l63.8-21.3c16.1,13.4,34.5,23.9,53.9,30.9l13.2,64.9h95.7l13.2-64.9
		c19.6-7,37.6-17.4,53.9-30.9l63.8,21.3l48-82l-50.6-43.8C813.3,591,814.3,580.8,814.3,570.7 M635.3,665.8
		c-52.5,0-95.1-42.7-95.1-95.1s42.7-95.1,95.1-95.1c52.5,0,95.1,42.7,95.1,95.1S687.8,665.8,635.3,665.8"/>
</g>
</svg>
)svg"},
        {"Folder-Search-2.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Generator: Adobe Illustrator 23.0.0, SVG Export Plug-In . SVG Version: 6.00 Build 0)  -->
<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
	 viewBox="0 0 1000 1000" style="enable-background:new 0 0 1000 1000;" xml:space="preserve">
<style type="text/css">
	.st0{fill-rule:evenodd;clip-rule:evenodd;}
</style>
<title>在当前文件夹里查找-2</title>
<g>
	<path d="M999.5,286.4c0-47.5-38.5-86-86-86l0,0H552v-0.3c-42.5-41.8-71.3-90.4-94.6-116.7c-10.9-12.2-26.4-19.2-42.8-19.2H86.5
		c-47.5,0-86,38.5-86,86l0,0v699.5c0,47.5,38.5,86,86,86l0,0h607.7v-78.6c-81.1,15.4-165-3.7-231.5-52.6
		c-86.2-61.5-131.9-165.2-119-270.3c15.7-149,141.8-265.2,295-265.2c163.9,0.1,296.8,133.1,296.6,297c0,32.7-5.4,65.1-16,96v0.4
		h80.1L999.5,286.4L999.5,286.4z"/>
	<path class="st0" d="M814.3,570.7c0-10.1-0.9-20.3-2.8-30.6l50.6-43.8l-48-82l-63.8,21.3c-16.1-13.4-34.5-23.9-53.9-30.9
		l-13.2-64.9h-95.7l-13.2,64.9c-19.6,7-37.6,17.4-53.9,30.9l-63.8-21.3l-48,82l50.6,43.8c-1.9,10.3-2.8,20.5-2.8,30.6
		s0.9,20.3,2.8,30.6l-50.6,43.8l48,82l63.8-21.3c16.1,13.4,34.5,23.9,53.9,30.9l13.2,64.9h95.7l13.2-64.9
		c19.6-7,37.6-17.4,53.9-30.9l63.8,21.3l48-82l-50.6-43.8C813.3,591,814.3,580.8,814.3,570.7 M635.3,665.8
		c-52.5,0-95.1-42.7-95.1-95.1s42.7-95.1,95.1-95.1c52.5,0,95.1,42.7,95.1,95.1S687.8,665.8,635.3,665.8"/>
</g>
</svg>
)svg"},
        {"Folder-Search-2", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Generator: Adobe Illustrator 23.0.0, SVG Export Plug-In . SVG Version: 6.00 Build 0)  -->
<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
	 viewBox="0 0 1000 1000" style="enable-background:new 0 0 1000 1000;" xml:space="preserve">
<style type="text/css">
	.st0{fill-rule:evenodd;clip-rule:evenodd;}
</style>
<title>在当前文件夹里查找-2</title>
<g>
	<path d="M999.5,286.4c0-47.5-38.5-86-86-86l0,0H552v-0.3c-42.5-41.8-71.3-90.4-94.6-116.7c-10.9-12.2-26.4-19.2-42.8-19.2H86.5
		c-47.5,0-86,38.5-86,86l0,0v699.5c0,47.5,38.5,86,86,86l0,0h607.7v-78.6c-81.1,15.4-165-3.7-231.5-52.6
		c-86.2-61.5-131.9-165.2-119-270.3c15.7-149,141.8-265.2,295-265.2c163.9,0.1,296.8,133.1,296.6,297c0,32.7-5.4,65.1-16,96v0.4
		h80.1L999.5,286.4L999.5,286.4z"/>
	<path class="st0" d="M814.3,570.7c0-10.1-0.9-20.3-2.8-30.6l50.6-43.8l-48-82l-63.8,21.3c-16.1-13.4-34.5-23.9-53.9-30.9
		l-13.2-64.9h-95.7l-13.2,64.9c-19.6,7-37.6,17.4-53.9,30.9l-63.8-21.3l-48,82l50.6,43.8c-1.9,10.3-2.8,20.5-2.8,30.6
		s0.9,20.3,2.8,30.6l-50.6,43.8l48,82l63.8-21.3c16.1,13.4,34.5,23.9,53.9,30.9l13.2,64.9h95.7l13.2-64.9
		c19.6-7,37.6-17.4,53.9-30.9l63.8,21.3l48-82l-50.6-43.8C813.3,591,814.3,580.8,814.3,570.7 M635.3,665.8
		c-52.5,0-95.1-42.7-95.1-95.1s42.7-95.1,95.1-95.1c52.5,0,95.1,42.7,95.1,95.1S687.8,665.8,635.3,665.8"/>
</g>
</svg>
)svg"},
        {"在当前文件夹里查找-2", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Generator: Adobe Illustrator 23.0.0, SVG Export Plug-In . SVG Version: 6.00 Build 0)  -->
<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
	 viewBox="0 0 1000 1000" style="enable-background:new 0 0 1000 1000;" xml:space="preserve">
<style type="text/css">
	.st0{fill-rule:evenodd;clip-rule:evenodd;}
</style>
<title>在当前文件夹里查找-2</title>
<g>
	<path d="M999.5,286.4c0-47.5-38.5-86-86-86l0,0H552v-0.3c-42.5-41.8-71.3-90.4-94.6-116.7c-10.9-12.2-26.4-19.2-42.8-19.2H86.5
		c-47.5,0-86,38.5-86,86l0,0v699.5c0,47.5,38.5,86,86,86l0,0h607.7v-78.6c-81.1,15.4-165-3.7-231.5-52.6
		c-86.2-61.5-131.9-165.2-119-270.3c15.7-149,141.8-265.2,295-265.2c163.9,0.1,296.8,133.1,296.6,297c0,32.7-5.4,65.1-16,96v0.4
		h80.1L999.5,286.4L999.5,286.4z"/>
	<path class="st0" d="M814.3,570.7c0-10.1-0.9-20.3-2.8-30.6l50.6-43.8l-48-82l-63.8,21.3c-16.1-13.4-34.5-23.9-53.9-30.9
		l-13.2-64.9h-95.7l-13.2,64.9c-19.6,7-37.6,17.4-53.9,30.9l-63.8-21.3l-48,82l50.6,43.8c-1.9,10.3-2.8,20.5-2.8,30.6
		s0.9,20.3,2.8,30.6l-50.6,43.8l48,82l63.8-21.3c16.1,13.4,34.5,23.9,53.9,30.9l13.2,64.9h95.7l13.2-64.9
		c19.6-7,37.6-17.4,53.9-30.9l63.8,21.3l48-82l-50.6-43.8C813.3,591,814.3,580.8,814.3,570.7 M635.3,665.8
		c-52.5,0-95.1-42.7-95.1-95.1s42.7-95.1,95.1-95.1c52.5,0,95.1,42.7,95.1,95.1S687.8,665.8,635.3,665.8"/>
</g>
</svg>
)svg"},
        {"Uncategorized-1.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Generator: Adobe Illustrator 23.0.0, SVG Export Plug-In . SVG Version: 6.00 Build 0)  -->
<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
	 viewBox="0 0 999 871.6" style="enable-background:new 0 0 999 871.6;" xml:space="preserve">
<title>未分类-1</title>
<g>
	<path d="M999,264.7v279.8h-80.1V294.6c0-19.8-16-35.8-35.8-35.8H722.8L531.3,748.9h104.4V829H86.1C38.5,829,0,790.5,0,743
		c0,0,0,0,0,0V128.6c0-47.5,38.5-86,86-86c0,0,0,0,0,0h328.1c16.4,0,32.5,6.5,42.8,19.2l0.1,0.1l93.4,115.5l1.1,1.4H913
		C960.5,178.7,999,217.2,999,264.7C999,264.7,999,264.7,999,264.7z"/>
	<path d="M551.5,178.4l0,0.3l-1.1-1.4C550.8,177.7,551.2,178.1,551.5,178.4z"/>
	<path d="M727.8,722.6c-3.6-9.7-5.4-20-5.4-30.4c0-25.5,12.1-48.5,36.2-68.9c15-12.7,25.1-23.3,30.4-31.7c5.2-8.2,8-17.7,7.9-27.4
		c0-10.7-3.8-19.5-11.3-26.4c-7.6-6.9-17.3-10.3-29.2-10.3c-27.9,0-53.9,10.7-77.9,32.2v-70.2c25.2-15.1,53.9-22.7,86.1-22.7
		c32.6,0,58.4,7.6,77.3,22.7c18.9,15.1,28.3,36.6,28.3,64.6c0,16.8-3.7,31.9-11.1,45.5c-7.4,13.6-21,28.5-40.8,44.6
		c-15.9,13.3-25.7,23.6-29.5,31c-3.7,7.3-5.7,15.4-5.7,23.6c0,8.3,2.1,16.5,6.1,23.8L727.8,722.6z M718.5,790.1
		c-0.2-10.7,4.5-20.9,12.7-27.8c8.5-7.3,18.7-11,30.8-11c11.6,0,21.6,3.7,30,11c8.2,6.9,12.8,17.1,12.6,27.8
		c0.2,10.5-4.1,20.6-11.8,27.8c-7.6,7.4-17.8,11.4-28.3,11.2c-14,0-25.2-3.9-33.5-11.6C722.6,809.8,718.5,800.6,718.5,790.1z"/>
</g>
</svg>
)svg"},
        {"Uncategorized-1", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Generator: Adobe Illustrator 23.0.0, SVG Export Plug-In . SVG Version: 6.00 Build 0)  -->
<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
	 viewBox="0 0 999 871.6" style="enable-background:new 0 0 999 871.6;" xml:space="preserve">
<title>未分类-1</title>
<g>
	<path d="M999,264.7v279.8h-80.1V294.6c0-19.8-16-35.8-35.8-35.8H722.8L531.3,748.9h104.4V829H86.1C38.5,829,0,790.5,0,743
		c0,0,0,0,0,0V128.6c0-47.5,38.5-86,86-86c0,0,0,0,0,0h328.1c16.4,0,32.5,6.5,42.8,19.2l0.1,0.1l93.4,115.5l1.1,1.4H913
		C960.5,178.7,999,217.2,999,264.7C999,264.7,999,264.7,999,264.7z"/>
	<path d="M551.5,178.4l0,0.3l-1.1-1.4C550.8,177.7,551.2,178.1,551.5,178.4z"/>
	<path d="M727.8,722.6c-3.6-9.7-5.4-20-5.4-30.4c0-25.5,12.1-48.5,36.2-68.9c15-12.7,25.1-23.3,30.4-31.7c5.2-8.2,8-17.7,7.9-27.4
		c0-10.7-3.8-19.5-11.3-26.4c-7.6-6.9-17.3-10.3-29.2-10.3c-27.9,0-53.9,10.7-77.9,32.2v-70.2c25.2-15.1,53.9-22.7,86.1-22.7
		c32.6,0,58.4,7.6,77.3,22.7c18.9,15.1,28.3,36.6,28.3,64.6c0,16.8-3.7,31.9-11.1,45.5c-7.4,13.6-21,28.5-40.8,44.6
		c-15.9,13.3-25.7,23.6-29.5,31c-3.7,7.3-5.7,15.4-5.7,23.6c0,8.3,2.1,16.5,6.1,23.8L727.8,722.6z M718.5,790.1
		c-0.2-10.7,4.5-20.9,12.7-27.8c8.5-7.3,18.7-11,30.8-11c11.6,0,21.6,3.7,30,11c8.2,6.9,12.8,17.1,12.6,27.8
		c0.2,10.5-4.1,20.6-11.8,27.8c-7.6,7.4-17.8,11.4-28.3,11.2c-14,0-25.2-3.9-33.5-11.6C722.6,809.8,718.5,800.6,718.5,790.1z"/>
</g>
</svg>
)svg"},
        {"Folder-sub.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Generator: Adobe Illustrator 23.0.0, SVG Export Plug-In . SVG Version: 6.00 Build 0)  -->
<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
	 viewBox="0 0 999 871.6" style="enable-background:new 0 0 999 871.6;" xml:space="preserve">
<title>未分类-1</title>
<path d="M999,264.7v279.8H890.6V323c0-19.8-16-35.8-35.8-35.8h-132L531.3,720.6h104.4v108.4H86.1c-47.5,0-86-38.5-86.1-86l0,0V128.5
	c0-47.5,38.5-86,86-86l0,0h328.1c16.4,0,32.5,6.5,42.8,19.2l0.1,0.1l93.4,115.5l1.1,1.4H913C960.5,178.7,999,217.2,999,264.7
	L999,264.7z"/>
<path d="M551.5,178.4v0.3l-1.1-1.4C550.8,177.7,551.2,178.1,551.5,178.4z"/>
<path d="M973.3,636.4v4.5c0,26.4-21.4,47.7-47.7,47.7H689.3c-26.4,0-47.7-21.4-47.7-47.7v-4.5c0-26.4,21.4-47.7,47.7-47.7h236.3
	C951.9,588.7,973.3,610.1,973.3,636.4z"/>
</svg>)svg"},
        {"Folder-sub", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Generator: Adobe Illustrator 23.0.0, SVG Export Plug-In . SVG Version: 6.00 Build 0)  -->
<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
	 viewBox="0 0 999 871.6" style="enable-background:new 0 0 999 871.6;" xml:space="preserve">
<title>未分类-1</title>
<path d="M999,264.7v279.8H890.6V323c0-19.8-16-35.8-35.8-35.8h-132L531.3,720.6h104.4v108.4H86.1c-47.5,0-86-38.5-86.1-86l0,0V128.5
	c0-47.5,38.5-86,86-86l0,0h328.1c16.4,0,32.5,6.5,42.8,19.2l0.1,0.1l93.4,115.5l1.1,1.4H913C960.5,178.7,999,217.2,999,264.7
	L999,264.7z"/>
<path d="M551.5,178.4v0.3l-1.1-1.4C550.8,177.7,551.2,178.1,551.5,178.4z"/>
<path d="M973.3,636.4v4.5c0,26.4-21.4,47.7-47.7,47.7H689.3c-26.4,0-47.7-21.4-47.7-47.7v-4.5c0-26.4,21.4-47.7,47.7-47.7h236.3
	C951.9,588.7,973.3,610.1,973.3,636.4z"/>
</svg>)svg"},
        {"Folder-add.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Generator: Adobe Illustrator 23.0.0, SVG Export Plug-In . SVG Version: 6.00 Build 0)  -->
<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
	 viewBox="0 0 999 871.6" style="enable-background:new 0 0 999 871.6;" xml:space="preserve">
<title>未分类-1</title>
<path d="M999,264.7v279.8H890.6V323c0-19.8-16-35.8-35.8-35.8h-132L531.3,720.6h104.4v108.4H86.1c-47.5,0-86-38.5-86.1-86l0,0V128.5
	c0-47.5,38.5-86,86-86l0,0h328.1c16.4,0,32.5,6.5,42.8,19.2l0.1,0.1l93.4,115.5l1.1,1.4H913C960.5,178.7,999,217.2,999,264.7
	L999,264.7z"/>
<path d="M551.5,178.4v0.3l-1.1-1.4C550.8,177.7,551.2,178.1,551.5,178.4z"/>
<path d="M973.3,660.9v4.5c0,26.4-21.4,47.7-47.7,47.7h-68.1v68.1c0,26.4-21.4,47.7-47.7,47.7h-4.5c-26.4,0-47.7-21.4-47.7-47.7
	v-68.1h-68.1c-26.4,0-47.7-21.4-47.7-47.7v-4.5c0-26.4,21.4-47.7,47.7-47.7h68.1v-68.2c0-26.4,21.4-47.7,47.7-47.7h4.5
	c26.4,0,47.7,21.4,47.7,47.7v68.2h68.1C951.9,613.2,973.3,634.6,973.3,660.9z"/>
</svg>)svg"},
        {"Folder-add", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Generator: Adobe Illustrator 23.0.0, SVG Export Plug-In . SVG Version: 6.00 Build 0)  -->
<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
	 viewBox="0 0 999 871.6" style="enable-background:new 0 0 999 871.6;" xml:space="preserve">
<title>未分类-1</title>
<path d="M999,264.7v279.8H890.6V323c0-19.8-16-35.8-35.8-35.8h-132L531.3,720.6h104.4v108.4H86.1c-47.5,0-86-38.5-86.1-86l0,0V128.5
	c0-47.5,38.5-86,86-86l0,0h328.1c16.4,0,32.5,6.5,42.8,19.2l0.1,0.1l93.4,115.5l1.1,1.4H913C960.5,178.7,999,217.2,999,264.7
	L999,264.7z"/>
<path d="M551.5,178.4v0.3l-1.1-1.4C550.8,177.7,551.2,178.1,551.5,178.4z"/>
<path d="M973.3,660.9v4.5c0,26.4-21.4,47.7-47.7,47.7h-68.1v68.1c0,26.4-21.4,47.7-47.7,47.7h-4.5c-26.4,0-47.7-21.4-47.7-47.7
	v-68.1h-68.1c-26.4,0-47.7-21.4-47.7-47.7v-4.5c0-26.4,21.4-47.7,47.7-47.7h68.1v-68.2c0-26.4,21.4-47.7,47.7-47.7h4.5
	c26.4,0,47.7,21.4,47.7,47.7v68.2h68.1C951.9,613.2,973.3,634.6,973.3,660.9z"/>
</svg>)svg"},
        {"Search-in-library.svg", R"svg(<svg fill="currentColor" id="图层_1" data-name="图层 1" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 1000 1000"><defs><style>.cls-1{fill:none;}</style></defs><title>在数据库里搜索</title><path d="M328.611,385.305c-90.4945,90.4945-90.50006,237.21291,0,327.713,81.44231,81.44231,208.42348,89.58924,298.97356,24.44092L746.091,855.96531a27.38948,27.38948,0,0,0,38.73014.00007l14.45025-14.45025a27.38462,27.38462,0,0,0,.00007-38.73028L680.765,684.27839c65.14825-90.55,57.00125-217.53112-24.44106-298.97343C565.82388,294.8049,419.10547,294.81046,328.611,385.305ZM603.149,659.843a156.52736,156.52736,0,1,1,0-221.36312A156.5278,156.5278,0,0,1,603.149,659.843Z"/><ellipse class="cls-1" cx="500" cy="159.09001" rx="404.54999" ry="68.18004"/><path d="M500,0C447.73,0,0,4.55,0,159.09V840.91C0,995.45,447.73,1000,500,1000s500-4.55,500-159.09V159.09C1000,4.55,552.27,0,500,0Zm0,90.91c222.73,0,370.45,40.91,404.55,68.18006-34.1,27.27-181.82,68.18-404.55,68.18S129.55,186.36,95.45,159.09C129.55,131.82,277.27,90.91,500,90.91ZM909.09,381.82c-11.68006,14.01-56.95,32.82-130.26,47.8a302.77941,302.77941,0,0,1,22.45,88.42c39.05-8,76.31-18.33,107.81-31.68V609.09c-11.44763,13.72717-55.16248,32.07226-125.92676,46.90069l75.34283,75.34284A458.07287,458.07287,0,0,0,909.09,713.64V836.36C886.36,863.64,736.36,909.09,500,909.09S113.64,863.64,90.91,836.36V713.64c52.34,22.17,120.6,36.04,186.55,44.71a304.95085,304.95085,0,0,1-64.79-103.23C144.36,640.45,102.13,622.55,90.91,609.09V486.36c30.47,12.91,66.34,23.01,104.01,30.9a302.58276,302.58276,0,0,1,22.69-88.37c-71.22-14.86-115.21-33.29-126.7-47.07V259.09c61.24,25.95,144.29,40.52,219.91,48.69a304.76282,304.76282,0,0,1,374.95.37c76.53-8.1,161.15-22.72,223.32-49.06Z"/></svg>)svg"},
        {"Search-in-library", R"svg(<svg fill="currentColor" id="图层_1" data-name="图层 1" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 1000 1000"><defs><style>.cls-1{fill:none;}</style></defs><title>在数据库里搜索</title><path d="M328.611,385.305c-90.4945,90.4945-90.50006,237.21291,0,327.713,81.44231,81.44231,208.42348,89.58924,298.97356,24.44092L746.091,855.96531a27.38948,27.38948,0,0,0,38.73014.00007l14.45025-14.45025a27.38462,27.38462,0,0,0,.00007-38.73028L680.765,684.27839c65.14825-90.55,57.00125-217.53112-24.44106-298.97343C565.82388,294.8049,419.10547,294.81046,328.611,385.305ZM603.149,659.843a156.52736,156.52736,0,1,1,0-221.36312A156.5278,156.5278,0,0,1,603.149,659.843Z"/><ellipse class="cls-1" cx="500" cy="159.09001" rx="404.54999" ry="68.18004"/><path d="M500,0C447.73,0,0,4.55,0,159.09V840.91C0,995.45,447.73,1000,500,1000s500-4.55,500-159.09V159.09C1000,4.55,552.27,0,500,0Zm0,90.91c222.73,0,370.45,40.91,404.55,68.18006-34.1,27.27-181.82,68.18-404.55,68.18S129.55,186.36,95.45,159.09C129.55,131.82,277.27,90.91,500,90.91ZM909.09,381.82c-11.68006,14.01-56.95,32.82-130.26,47.8a302.77941,302.77941,0,0,1,22.45,88.42c39.05-8,76.31-18.33,107.81-31.68V609.09c-11.44763,13.72717-55.16248,32.07226-125.92676,46.90069l75.34283,75.34284A458.07287,458.07287,0,0,0,909.09,713.64V836.36C886.36,863.64,736.36,909.09,500,909.09S113.64,863.64,90.91,836.36V713.64c52.34,22.17,120.6,36.04,186.55,44.71a304.95085,304.95085,0,0,1-64.79-103.23C144.36,640.45,102.13,622.55,90.91,609.09V486.36c30.47,12.91,66.34,23.01,104.01,30.9a302.58276,302.58276,0,0,1,22.69-88.37c-71.22-14.86-115.21-33.29-126.7-47.07V259.09c61.24,25.95,144.29,40.52,219.91,48.69a304.76282,304.76282,0,0,1,374.95.37c76.53-8.1,161.15-22.72,223.32-49.06Z"/></svg>)svg"},
        {"Folder-Seach.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Generator: Adobe Illustrator 23.0.0, SVG Export Plug-In . SVG Version: 6.00 Build 0)  -->
<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
	 viewBox="0 0 1000 1000" style="enable-background:new 0 0 1000 1000;" xml:space="preserve">
<title>在当前文件夹里查找-2</title>
<g>
	<path d="M703.1,364.3c-111.2-35.6-230.2,25.8-265.8,137s25.8,230.2,137,265.8c61.8,19.7,129.2,10,182.9-26.3l135.6,135.6
		c15.6,15.6,40.9,15.6,56.6,0c15.6-15.6,15.6-40.9,0-56.6L813.9,684.3c65.4-96.8,40-228.3-56.7-293.7
		C740.5,379.3,722.3,370.5,703.1,364.3L703.1,364.3z M759.8,565.7c0,29.1-10.4,57.2-29.5,79.2c-1.9,2.2-3.9,4.3-6,6.4
		c-2.1,2.1-4.2,4.1-6.4,6c-50.6,43.7-127,38.2-170.8-12.4c-43.7-50.6-38.2-127,12.4-170.8s127-38.2,170.8,12.4
		C749.4,508.6,759.8,536.7,759.8,565.7z"/>
	<path d="M999.5,286.4c0-47.5-38.5-86-86-86c0,0,0,0,0,0H552l0-0.3c-42.5-41.8-71.3-90.4-94.6-116.7c-10.9-12.2-26.4-19.2-42.8-19.2
		H86.5c-47.5,0-86,38.5-86,86c0,0,0,0,0,0v699.5c0,47.5,38.5,86,86,86c0,0,0,0,0,0h607.7v-78.6c-81.1,15.4-165-3.7-231.5-52.6
		c-86.2-61.5-131.9-165.2-119-270.3c15.7-149,141.8-265.2,295-265.2c163.9,0.1,296.8,133.1,296.6,297c0,32.7-5.4,65.1-16,96v0.4
		h80.1V286.4z"/>
</g>
</svg>
)svg"},
        {"Folder-Seach", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Generator: Adobe Illustrator 23.0.0, SVG Export Plug-In . SVG Version: 6.00 Build 0)  -->
<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
	 viewBox="0 0 1000 1000" style="enable-background:new 0 0 1000 1000;" xml:space="preserve">
<title>在当前文件夹里查找-2</title>
<g>
	<path d="M703.1,364.3c-111.2-35.6-230.2,25.8-265.8,137s25.8,230.2,137,265.8c61.8,19.7,129.2,10,182.9-26.3l135.6,135.6
		c15.6,15.6,40.9,15.6,56.6,0c15.6-15.6,15.6-40.9,0-56.6L813.9,684.3c65.4-96.8,40-228.3-56.7-293.7
		C740.5,379.3,722.3,370.5,703.1,364.3L703.1,364.3z M759.8,565.7c0,29.1-10.4,57.2-29.5,79.2c-1.9,2.2-3.9,4.3-6,6.4
		c-2.1,2.1-4.2,4.1-6.4,6c-50.6,43.7-127,38.2-170.8-12.4c-43.7-50.6-38.2-127,12.4-170.8s127-38.2,170.8,12.4
		C749.4,508.6,759.8,536.7,759.8,565.7z"/>
	<path d="M999.5,286.4c0-47.5-38.5-86-86-86c0,0,0,0,0,0H552l0-0.3c-42.5-41.8-71.3-90.4-94.6-116.7c-10.9-12.2-26.4-19.2-42.8-19.2
		H86.5c-47.5,0-86,38.5-86,86c0,0,0,0,0,0v699.5c0,47.5,38.5,86,86,86c0,0,0,0,0,0h607.7v-78.6c-81.1,15.4-165-3.7-231.5-52.6
		c-86.2-61.5-131.9-165.2-119-270.3c15.7-149,141.8-265.2,295-265.2c163.9,0.1,296.8,133.1,296.6,297c0,32.7-5.4,65.1-16,96v0.4
		h80.1V286.4z"/>
</g>
</svg>
)svg"},
        {"Clear.svg", R"svg(<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px" viewBox="0 0 1000 1000" style="enable-background:new 0 0 1000 1000;" xml:space="preserve"><style type="text/css">.st0{fill:#FFFFFF;}</style><circle class="st0" cx="500" cy="500" r="500"/><path d="M783.4,782.2c-14.7,15-34.2,22.6-53.6,22.6c-19.2,0-38.4-7.3-53-22L543,649.1c-23.8-23.8-62.3-23.8-86,0L323.9,782.2c-29.2,29.2-76.9,30.1-106.3,1.1c-29.7-29.3-29.8-77-0.4-106.5L350.9,543c23.8-23.8,23.8-62.3,0-86L217.8,323.9c-29.2-29.2-30.1-76.9-1.1-106.3c29.3-29.7,77-29.8,106.5-0.4L457,350.9c23.8,23.8,62.3,23.8,86,0l133.1-133.1c29.2-29.2,76.6-30.1,106.1-1.3c15,14.7,22.6,34.2,22.6,53.6c0,19.2-7.3,38.4-22,53L649.1,457c-23.8,23.8-23.8,62.3,0,86l133.1,133.1C811.3,705.3,812.3,752.7,783.4,782.2z"/></svg>)svg"},
        {"Clear", R"svg(<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px" viewBox="0 0 1000 1000" style="enable-background:new 0 0 1000 1000;" xml:space="preserve"><style type="text/css">.st0{fill:#FFFFFF;}</style><circle class="st0" cx="500" cy="500" r="500"/><path d="M783.4,782.2c-14.7,15-34.2,22.6-53.6,22.6c-19.2,0-38.4-7.3-53-22L543,649.1c-23.8-23.8-62.3-23.8-86,0L323.9,782.2c-29.2,29.2-76.9,30.1-106.3,1.1c-29.7-29.3-29.8-77-0.4-106.5L350.9,543c23.8-23.8,23.8-62.3,0-86L217.8,323.9c-29.2-29.2-30.1-76.9-1.1-106.3c29.3-29.7,77-29.8,106.5-0.4L457,350.9c23.8,23.8,62.3,23.8,86,0l133.1-133.1c29.2-29.2,76.6-30.1,106.1-1.3c15,14.7,22.6,34.2,22.6,53.6c0,19.2-7.3,38.4-22,53L649.1,457c-23.8,23.8-23.8,62.3,0,86l133.1,133.1C811.3,705.3,812.3,752.7,783.4,782.2z"/></svg>)svg"},
        {"clear.svg", R"svg(<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px" viewBox="0 0 1000 1000" style="enable-background:new 0 0 1000 1000;" xml:space="preserve"><style type="text/css">.st0{fill:#FFFFFF;}</style><circle class="st0" cx="500" cy="500" r="500"/><path d="M783.4,782.2c-14.7,15-34.2,22.6-53.6,22.6c-19.2,0-38.4-7.3-53-22L543,649.1c-23.8-23.8-62.3-23.8-86,0L323.9,782.2c-29.2,29.2-76.9,30.1-106.3,1.1c-29.7-29.3-29.8-77-0.4-106.5L350.9,543c23.8-23.8,23.8-62.3,0-86L217.8,323.9c-29.2-29.2-30.1-76.9-1.1-106.3c29.3-29.7,77-29.8,106.5-0.4L457,350.9c23.8,23.8,62.3,23.8,86,0l133.1-133.1c29.2-29.2,76.6-30.1,106.1-1.3c15,14.7,22.6,34.2,22.6,53.6c0,19.2-7.3,38.4-22,53L649.1,457c-23.8,23.8-23.8,62.3,0,86l133.1,133.1C811.3,705.3,812.3,752.7,783.4,782.2z"/></svg>)svg"},
        {"clear", R"svg(<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px" viewBox="0 0 1000 1000" style="enable-background:new 0 0 1000 1000;" xml:space="preserve"><style type="text/css">.st0{fill:#FFFFFF;}</style><circle class="st0" cx="500" cy="500" r="500"/><path d="M783.4,782.2c-14.7,15-34.2,22.6-53.6,22.6c-19.2,0-38.4-7.3-53-22L543,649.1c-23.8-23.8-62.3-23.8-86,0L323.9,782.2c-29.2,29.2-76.9,30.1-106.3,1.1c-29.7-29.3-29.8-77-0.4-106.5L350.9,543c23.8-23.8,23.8-62.3,0-86L217.8,323.9c-29.2-29.2-30.1-76.9-1.1-106.3c29.3-29.7,77-29.8,106.5-0.4L457,350.9c23.8,23.8,62.3,23.8,86,0l133.1-133.1c29.2-29.2,76.6-30.1,106.1-1.3c15,14.7,22.6,34.2,22.6,53.6c0,19.2-7.3,38.4-22,53L649.1,457c-23.8,23.8-23.8,62.3,0,86l133.1,133.1C811.3,705.3,812.3,752.7,783.4,782.2z"/></svg>)svg"},
        {"clear-2.svg", R"svg(<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px" viewBox="0 0 1000 1000" style="enable-background:new 0 0 1000 1000;" xml:space="preserve"><style type="text/css">.st0{fill:#4D4D4D;}.st1{fill:#FFFFFF;}</style><circle class="st0" cx="500" cy="500" r="500"/><path class="st1" d="M796,685.3L685.3,796c-0.7,0.7-1.8,0.7-2.5,0L500,613.1L317.2,796c-0.7,0.7-1.8,0.7-2.5,0L204,685.3c-0.7-0.7-0.7-1.8,0-2.5L386.9,500L204,317.2c-0.7-0.7-0.7-1.8,0-2.5L314.7,204c0.7-0.7,1.8-0.7,2.5,0L500,386.9L682.8,204c0.7-0.7,1.8-0.7,2.5,0L796,314.7c0.7,0.7,0.7,1.8,0,2.5L613.1,500L796,682.8C796.6,683.5,796.6,684.6,796,685.3z"/></svg>)svg"},
        {"clear-2", R"svg(<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px" viewBox="0 0 1000 1000" style="enable-background:new 0 0 1000 1000;" xml:space="preserve"><style type="text/css">.st0{fill:#4D4D4D;}.st1{fill:#FFFFFF;}</style><circle class="st0" cx="500" cy="500" r="500"/><path class="st1" d="M796,685.3L685.3,796c-0.7,0.7-1.8,0.7-2.5,0L500,613.1L317.2,796c-0.7,0.7-1.8,0.7-2.5,0L204,685.3c-0.7-0.7-0.7-1.8,0-2.5L386.9,500L204,317.2c-0.7-0.7-0.7-1.8,0-2.5L314.7,204c0.7-0.7,1.8-0.7,2.5,0L500,386.9L682.8,204c0.7-0.7,1.8-0.7,2.5,0L796,314.7c0.7,0.7,0.7,1.8,0,2.5L613.1,500L796,682.8C796.6,683.5,796.6,684.6,796,685.3z"/></svg>)svg"},
        {"Clear-2.svg", R"svg(<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px" viewBox="0 0 1000 1000" style="enable-background:new 0 0 1000 1000;" xml:space="preserve"><style type="text/css">.st0{fill:#4D4D4D;}.st1{fill:#FFFFFF;}</style><circle class="st0" cx="500" cy="500" r="500"/><path class="st1" d="M796,685.3L685.3,796c-0.7,0.7-1.8,0.7-2.5,0L500,613.1L317.2,796c-0.7,0.7-1.8,0.7-2.5,0L204,685.3c-0.7-0.7-0.7-1.8,0-2.5L386.9,500L204,317.2c-0.7-0.7-0.7-1.8,0-2.5L314.7,204c0.7-0.7,1.8-0.7,2.5,0L500,386.9L682.8,204c0.7-0.7,1.8-0.7,2.5,0L796,314.7c0.7,0.7,0.7,1.8,0,2.5L613.1,500L796,682.8C796.6,683.5,796.6,684.6,796,685.3z"/></svg>)svg"},
        {"Clear-2", R"svg(<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px" viewBox="0 0 1000 1000" style="enable-background:new 0 0 1000 1000;" xml:space="preserve"><style type="text/css">.st0{fill:#4D4D4D;}.st1{fill:#FFFFFF;}</style><circle class="st0" cx="500" cy="500" r="500"/><path class="st1" d="M796,685.3L685.3,796c-0.7,0.7-1.8,0.7-2.5,0L500,613.1L317.2,796c-0.7,0.7-1.8,0.7-2.5,0L204,685.3c-0.7-0.7-0.7-1.8,0-2.5L386.9,500L204,317.2c-0.7-0.7-0.7-1.8,0-2.5L314.7,204c0.7-0.7,1.8-0.7,2.5,0L500,386.9L682.8,204c0.7-0.7,1.8-0.7,2.5,0L796,314.7c0.7,0.7,0.7,1.8,0,2.5L613.1,500L796,682.8C796.6,683.5,796.6,684.6,796,685.3z"/></svg>)svg"},
        {"Favorites.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px" viewBox="0 0 1000 1000" style="enable-background:new 0 0 1000 1000;" xml:space="preserve">
<path d="M999.5,328.8v109.7h-80.1v-79.8c0-19.8-16-35.8-35.8-35.8H553.2L361.7,813h274.5v80.1H86.6c-47.6,0-86.1-38.5-86.1-86l0,0 V192.7c0-47.5,38.5-86,86-86l0,0h328.1c16.4,0,32.5,6.5,42.8,19.2l0.1,0.1l93.4,115.5l1.1,1.4h361.5 C961,242.8,999.5,281.3,999.5,328.8L999.5,328.8z"/>
<path d="M552,242.5v0.3l-1.1-1.4C551.3,241.8,551.7,242.2,552,242.5z"/>
<path d="M746.3,358.7c8.4,0,16.2,4.8,19.9,12.4l55.6,112.8l124.4,18.1c12.1,1.8,20.5,13,18.8,25.1c-0.7,4.8-3,9.3-6.5,12.7l-90,87.8 l21.3,123.9c2,12.1-6.1,23.5-18.2,25.6c-4.8,0.8-9.7,0-14-2.2l-111.3-58.5l-111.3,58.5c-10.8,5.7-24.2,1.5-29.9-9.3 c-2.3-4.3-3-9.2-2.2-14l21.3-124L534,539.7c-8.8-8.5-9-22.6-0.4-31.3c3.4-3.5,7.9-5.8,12.7-6.5l124.4-18.1l55.7-112.8 C730.1,363.5,737.8,358.7,746.3,358.7z"/>
</svg>)svg"},
        {"Favorites", R"svg(<?xml version="1.0" encoding="utf-8"?>
<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px" viewBox="0 0 1000 1000" style="enable-background:new 0 0 1000 1000;" xml:space="preserve">
<path d="M999.5,328.8v109.7h-80.1v-79.8c0-19.8-16-35.8-35.8-35.8H553.2L361.7,813h274.5v80.1H86.6c-47.6,0-86.1-38.5-86.1-86l0,0 V192.7c0-47.5,38.5-86,86-86l0,0h328.1c16.4,0,32.5,6.5,42.8,19.2l0.1,0.1l93.4,115.5l1.1,1.4h361.5 C961,242.8,999.5,281.3,999.5,328.8L999.5,328.8z"/>
<path d="M552,242.5v0.3l-1.1-1.4C551.3,241.8,551.7,242.2,552,242.5z"/>
<path d="M746.3,358.7c8.4,0,16.2,4.8,19.9,12.4l55.6,112.8l124.4,18.1c12.1,1.8,20.5,13,18.8,25.1c-0.7,4.8-3,9.3-6.5,12.7l-90,87.8 l21.3,123.9c2,12.1-6.1,23.5-18.2,25.6c-4.8,0.8-9.7,0-14-2.2l-111.3-58.5l-111.3,58.5c-10.8,5.7-24.2,1.5-29.9-9.3 c-2.3-4.3-3-9.2-2.2-14l21.3-124L534,539.7c-8.8-8.5-9-22.6-0.4-31.3c3.4-3.5,7.9-5.8,12.7-6.5l124.4-18.1l55.7-112.8 C730.1,363.5,737.8,358.7,746.3,358.7z"/>
</svg>)svg"},
        {"favorites.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px" viewBox="0 0 1000 1000" style="enable-background:new 0 0 1000 1000;" xml:space="preserve">
<path d="M999.5,328.8v109.7h-80.1v-79.8c0-19.8-16-35.8-35.8-35.8H553.2L361.7,813h274.5v80.1H86.6c-47.6,0-86.1-38.5-86.1-86l0,0 V192.7c0-47.5,38.5-86,86-86l0,0h328.1c16.4,0,32.5,6.5,42.8,19.2l0.1,0.1l93.4,115.5l1.1,1.4h361.5 C961,242.8,999.5,281.3,999.5,328.8L999.5,328.8z"/>
<path d="M552,242.5v0.3l-1.1-1.4C551.3,241.8,551.7,242.2,552,242.5z"/>
<path d="M746.3,358.7c8.4,0,16.2,4.8,19.9,12.4l55.6,112.8l124.4,18.1c12.1,1.8,20.5,13,18.8,25.1c-0.7,4.8-3,9.3-6.5,12.7l-90,87.8 l21.3,123.9c2,12.1-6.1,23.5-18.2,25.6c-4.8,0.8-9.7,0-14-2.2l-111.3-58.5l-111.3,58.5c-10.8,5.7-24.2,1.5-29.9-9.3 c-2.3-4.3-3-9.2-2.2-14l21.3-124L534,539.7c-8.8-8.5-9-22.6-0.4-31.3c3.4-3.5,7.9-5.8,12.7-6.5l124.4-18.1l55.7-112.8 C730.1,363.5,737.8,358.7,746.3,358.7z"/>
</svg>)svg"},
        {"favorites", R"svg(<?xml version="1.0" encoding="utf-8"?>
<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px" viewBox="0 0 1000 1000" style="enable-background:new 0 0 1000 1000;" xml:space="preserve">
<path d="M999.5,328.8v109.7h-80.1v-79.8c0-19.8-16-35.8-35.8-35.8H553.2L361.7,813h274.5v80.1H86.6c-47.6,0-86.1-38.5-86.1-86l0,0 V192.7c0-47.5,38.5-86,86-86l0,0h328.1c16.4,0,32.5,6.5,42.8,19.2l0.1,0.1l93.4,115.5l1.1,1.4h361.5 C961,242.8,999.5,281.3,999.5,328.8L999.5,328.8z"/>
<path d="M552,242.5v0.3l-1.1-1.4C551.3,241.8,551.7,242.2,552,242.5z"/>
<path d="M746.3,358.7c8.4,0,16.2,4.8,19.9,12.4l55.6,112.8l124.4,18.1c12.1,1.8,20.5,13,18.8,25.1c-0.7,4.8-3,9.3-6.5,12.7l-90,87.8 l21.3,123.9c2,12.1-6.1,23.5-18.2,25.6c-4.8,0.8-9.7,0-14-2.2l-111.3-58.5l-111.3,58.5c-10.8,5.7-24.2,1.5-29.9-9.3 c-2.3-4.3-3-9.2-2.2-14l21.3-124L534,539.7c-8.8-8.5-9-22.6-0.4-31.3c3.4-3.5,7.9-5.8,12.7-6.5l124.4-18.1l55.7-112.8 C730.1,363.5,737.8,358.7,746.3,358.7z"/>
</svg>)svg"},
        {"Destination.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Generator: Adobe Illustrator 23.0.0, SVG Export Plug-In . SVG Version: 6.00 Build 0)  -->
<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
	 viewBox="0 0 1000 1000" style="enable-background:new 0 0 1000 1000;" xml:space="preserve">
<style type="text/css">
	.st0{fill-rule:evenodd;clip-rule:evenodd;}
</style>
<g>
	<path class="st0" d="M264.6,59.4v800h-47.1v-800C217.5,59.4,264.6,59.4,264.6,59.4z M311.7,87.9c128.6-34,206.7,11,282.2,55.4
		c84.8,50,172.3,101.7,347.4,57.9c8.4-2.1,17.4,0.6,23.2,7.2c5.8,6.5,7.5,15.8,4.5,24c-84.8,226-210.1,230.3-331.2,234.5
		c-93,3.3-189.3,6.6-286,103.3l-40.1,40.1V87.9z"/>
	<ellipse class="st0" cx="241.1" cy="910" rx="211.5" ry="30.6"/>
</g>
</svg>)svg"},
        {"Destination", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Generator: Adobe Illustrator 23.0.0, SVG Export Plug-In . SVG Version: 6.00 Build 0)  -->
<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
	 viewBox="0 0 1000 1000" style="enable-background:new 0 0 1000 1000;" xml:space="preserve">
<style type="text/css">
	.st0{fill-rule:evenodd;clip-rule:evenodd;}
</style>
<g>
	<path class="st0" d="M264.6,59.4v800h-47.1v-800C217.5,59.4,264.6,59.4,264.6,59.4z M311.7,87.9c128.6-34,206.7,11,282.2,55.4
		c84.8,50,172.3,101.7,347.4,57.9c8.4-2.1,17.4,0.6,23.2,7.2c5.8,6.5,7.5,15.8,4.5,24c-84.8,226-210.1,230.3-331.2,234.5
		c-93,3.3-189.3,6.6-286,103.3l-40.1,40.1V87.9z"/>
	<ellipse class="st0" cx="241.1" cy="910" rx="211.5" ry="30.6"/>
</g>
</svg>)svg"},
        {"destination", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Generator: Adobe Illustrator 23.0.0, SVG Export Plug-In . SVG Version: 6.00 Build 0)  -->
<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
	 viewBox="0 0 1000 1000" style="enable-background:new 0 0 1000 1000;" xml:space="preserve">
<style type="text/css">
	.st0{fill-rule:evenodd;clip-rule:evenodd;}
</style>
<g>
	<path class="st0" d="M264.6,59.4v800h-47.1v-800C217.5,59.4,264.6,59.4,264.6,59.4z M311.7,87.9c128.6-34,206.7,11,282.2,55.4
		c84.8,50,172.3,101.7,347.4,57.9c8.4-2.1,17.4,0.6,23.2,7.2c5.8,6.5,7.5,15.8,4.5,24c-84.8,226-210.1,230.3-331.2,234.5
		c-93,3.3-189.3,6.6-286,103.3l-40.1,40.1V87.9z"/>
	<ellipse class="st0" cx="241.1" cy="910" rx="211.5" ry="30.6"/>
</g>
</svg>)svg"},
        {"Data-Classification.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Generator: Adobe Illustrator 23.0.0, SVG Export Plug-In . SVG Version: 6.00 Build 0)  -->
<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
	 viewBox="0 0 1000 1000" style="enable-background:new 0 0 1000 1000;" xml:space="preserve">
<style type="text/css">
	.st0{fill:none;}
</style>
<path d="M999.5,328.9v109.7h-80.1v-79.8c0-19.8-16-35.8-35.8-35.8H553.2L361.7,813.1h274.5v80.1H86.6c-47.6,0-86.1-38.5-86.1-86l0,0
	V192.8c0-47.5,38.5-86,86-86l0,0h328.1c16.4,0,32.5,6.5,42.8,19.2l0.1,0.1l93.4,115.5l1.1,1.4h361.5
	C961,242.9,999.5,281.4,999.5,328.9L999.5,328.9z"/>
<g id="invisible_box">
	<rect x="528" y="359.6" class="st0" width="430.5" height="430.5"/>
</g>
<path d="M1000,823.3c0,46-37.3,83.3-83.3,83.3c-46,0-83.3-37.3-83.3-83.3c0-31.4,17.4-58.7,43-72.9v-50.9
	c0-19.9-16.2-36.1-36.1-36.1h-38.9v0.5h-84.2c-0.6,0-1.3,0-1.9-0.1v86.6c25.7,14.2,43,41.5,43,72.9c0,46-37.3,83.3-83.3,83.3
	c-46,0-83.3-37.3-83.3-83.3c0-31.4,17.4-58.7,43-72.9V528.7c-25.7-14.2-43-41.5-43-72.9c0-46,37.3-83.3,83.3-83.3
	c46,0,83.3,37.3,83.3,83.3c0,31.4-17.4,58.7-43,72.9v19c0,19.9,16.2,36.1,36.1,36.1h50v-0.5h73.1c0.6,0,1.3,0,1.9,0.1
	c44.6,1,80.5,37.5,80.5,82.3v75.3c0,3.1-0.2,6.1-0.5,9.1C982.4,764.2,1000,791.7,1000,823.3z"/>
</svg>)svg"},
        {"smart-folder-1.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Generator: Adobe Illustrator 23.0.0, SVG Export Plug-In . SVG Version: 6.00 Build 0)  -->
<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
	 viewBox="0 0 1000 1000" style="enable-background:new 0 0 1000 1000;" xml:space="preserve">
<path d="M999.5,328.9v109.7h-80.1v-79.8c0-19.8-16-35.8-35.8-35.8H553.2L361.7,813.1h274.5v80.1H86.6c-47.6,0-86.1-38.5-86.1-86l0,0
	V192.8c0-47.5,38.5-86,86-86l0,0h328.1c16.4,0,32.5,6.5,42.8,19.2l0.1,0.1l93.4,115.5l1.1,1.4h361.5
	C961,242.9,999.5,281.4,999.5,328.9L999.5,328.9z"/>
<path d="M926.6,673.3l-42.2-24.3c3.6-13.3,5.5-27.3,5.5-41.7c0-14.4-1.9-28.4-5.5-41.7l42.2-24.3c7-4,9.4-13,5.3-19.9l-25.7-44.5
	c-4-7-13-9.4-19.9-5.3l-42.2,24.4c-19.6-19.8-44.4-34.4-72.1-41.7v-48.7c0-8-6.5-14.6-14.6-14.6h-51.4c-8.1,0-14.6,6.5-14.6,14.6
	v48.7c-27.8,7.3-52.5,21.9-72.1,41.7L577,471.4c-7-4-15.9-1.6-19.9,5.3l-25.7,44.5c-4,7-1.6,15.9,5.3,19.9l42.2,24.3
	c-3.6,13.3-5.5,27.3-5.5,41.7c0,14.4,1.9,28.4,5.5,41.7l-42.2,24.3c-7,4-9.4,13-5.3,19.9l25.7,44.5c4,7,12.9,9.4,19.9,5.3l42.2-24.4
	c19.6,19.8,44.4,34.4,72.1,41.7v48.7c0,8.1,6.5,14.6,14.6,14.6h51.4c8,0,14.6-6.5,14.6-14.6v-48.7c27.8-7.3,52.5-21.9,72.1-41.7
	l42.2,24.4c7,4,15.9,1.6,19.9-5.3l25.7-44.5C936,686.2,933.6,677.3,926.6,673.3z M820.3,611.9L820.3,611.9
	c-1.5,29.3-17.2,54.8-40.3,69.8c-2.6,1.7-5.3,3.3-8.1,4.7c-12.1,6.2-25.8,9.6-40.3,9.6s-28.2-3.5-40.3-9.7c-2.8-1.4-5.5-3-8-4.7
	c-23.1-15-38.8-40.5-40.3-69.8c-0.1-1.5-0.1-3.1-0.1-4.6s0-3.1,0.1-4.7c1.5-29.3,17.2-54.8,40.3-69.8c2.6-1.7,5.3-3.3,8-4.7
	c12.1-6.2,25.8-9.7,40.3-9.7s28.2,3.5,40.3,9.7c2.8,1.4,5.5,3,8.1,4.7c23.1,15,38.8,40.5,40.3,69.8c0.1,1.5,0.1,3.1,0.1,4.7
	S820.4,610.4,820.3,611.9z"/>
</svg>)svg"},
        {"Database-1.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Generator: Adobe Illustrator 23.0.0, SVG Export Plug-In . SVG Version: 6.00 Build 0)  -->
<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
	 viewBox="0 0 1000 1000" style="enable-background:new 0 0 1000 1000;" xml:space="preserve">
<g>
	<path d="M499.9,0C291.4,0,122.4,83.5,122.4,186.6v629.5c0,103.1,169,183.9,377.6,183.9c39,0,76.6-2.8,112-8.1
		c-48.2-13.6-89.6-43.5-117.8-83.3c-157-1.1-283.3-48.4-283.3-106.6v-85c15.3,9,32.3,17.3,50.8,24.7c54,21.8,120.3,36.2,193,40.5
		c0.2-28,5.8-54.8,15.8-79.3c-16.8-0.6-33.2-1.8-49.1-3.4c-65.7-6.8-122-22-159.7-42.2c-32-17.1-50.8-37.9-50.8-60.3v-85
		c15.3,9,32.3,17.3,50.8,24.7c65,26.2,147.9,41.8,238.2,41.8s173.2-15.7,238.2-41.8c18.5-7.4,35.5-15.7,50.8-24.7v85
		c0,2.1-0.2,4.1-0.5,6.2c42.4,27.6,74.4,69.8,89.2,119.4V186.6C877.5,83.5,708.4,0,499.9,0z M499.7,80c159.6,0,289,47.7,289,106.6
		c0.2,22.3-18.5,43.1-50.6,60.3c-37.8,20.2-94,35.4-159.8,42.2c-24.9,2.6-51.3,4-78.5,4s-53.5-1.4-78.5-4
		c-65.7-6.8-122-22-159.8-42.2c-32-17.2-50.8-37.9-50.8-60.3C210.7,127.8,340.1,80,499.7,80z M738.1,452
		c-37.8,20.2-94,35.4-159.8,42.2c-24.9,2.6-51.3,4-78.5,4s-53.5-1.4-78.5-4c-65.7-6.8-122-22-159.8-42.2
		c-32-17.1-50.8-37.9-50.8-60.3v-85.1c15.3,9,32.3,17.3,50.8,24.7c65,26.2,147.9,41.8,238.2,41.8s173.2-15.7,238.2-41.8
		c18.5-7.4,35.5-15.7,50.8-24.7v85.1h0.2C788.9,414.1,770.1,435,738.1,452z"/>
</g>
<g>
	<path d="M818.8,757.8v70.5c0,6.5-5.3,11.8-11.8,11.8h-88.9V929c0,6.5-5.3,11.8-11.8,11.8h-70.5c-6.5,0-11.8-5.3-11.8-11.8v-88.9
		h-88.9c-6.5,0-11.8-5.3-11.8-11.8v-70.5c0-6.5,5.3-11.8,11.8-11.8H624v-88.9c0-6.5,5.3-11.8,11.8-11.8h70.5
		c6.5,0,11.8,5.3,11.8,11.8V746H807C813.5,746,818.8,751.3,818.8,757.8z"/>
</g>
</svg>)svg"},
        {"Export-1.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Generator: Adobe Illustrator 23.0.0, SVG Export Plug-In . SVG Version: 6.00 Build 0)  -->
<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
	 viewBox="0 0 1000 1000" style="enable-background:new 0 0 1000 1000;" xml:space="preserve">
<path d="M999.5,328.8v109.7h-80.1v-79.8c0-19.8-16-35.8-35.8-35.8H553.2L361.7,813h274.5v80.1H86.6c-47.6,0-86.1-38.5-86.1-86l0,0
	V192.7c0-47.5,38.5-86,86-86l0,0h328.1c16.4,0,32.5,6.5,42.8,19.2l0.1,0.1l93.4,115.5l1.1,1.4h361.5
	C961,242.8,999.5,281.3,999.5,328.8L999.5,328.8z"/>
<path d="M552,242.5v0.3l-1.1-1.4C551.3,241.8,551.7,242.2,552,242.5z"/>
<polygon points="765.6,749.5 765.6,654.7 522.2,654.7 522.2,574.1 765.6,574.1 765.6,479.4 999.5,614.4 "/>
</svg>)svg"},
        {"rename-1.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Generator: Adobe Illustrator 23.0.0, SVG Export Plug-In . SVG Version: 6.00 Build 0)  -->
<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
	 viewBox="0 0 1000 1000" style="enable-background:new 0 0 1000 1000;" xml:space="preserve">
<line x1="82.2" y1="586.9" x2="82.7" y2="586.9"/>
<g>
	<path d="M320.2,256.3v487.6h37.9c5,0,9.2,4.1,9.2,9.2v61.8c0,5-4.1,9.2-9.2,9.2H202.6c-5,0-9.2-4.1-9.2-9.2v-61.8
		c0-5,4.1-9.2,9.2-9.2h37.9V256.3h-37.9c-5,0-9.2-4.1-9.2-9.2v-62.1c0-5,4.1-9.2,9.2-9.2H358c5,0,9.2,4.1,9.2,9.2v62.1
		c0,5-4.1,9.2-9.2,9.2L320.2,256.3L320.2,256.3z"/>
</g>
<g>
	<path d="M195.6,596.9c-19.4,0-35.1-15.7-35.1-35.1V438.2c0-19.4,15.7-35.1,35.1-35.1h45.7v-80.6h-89.9c-38,0-68.7,30.8-68.7,68.7
		v217.7c0,38,30.8,68.7,68.7,68.7h89.9v-80.6L195.6,596.9L195.6,596.9z"/>
	<path d="M851.4,322.5h-454v80.6h407c19.4,0,35.1,15.7,35.1,35.1v123.6c0,19.4-15.7,35.1-35.1,35.1h-407v80.6h454
		c38,0,68.7-30.8,68.7-68.7V391.2C920.1,353.2,889.3,322.5,851.4,322.5z"/>
</g>
</svg>)svg"},
        {"link-1.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Generator: Adobe Illustrator 23.0.0, SVG Export Plug-In . SVG Version: 6.00 Build 0)  -->
<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
	 viewBox="0 0 1000 1000" style="enable-background:new 0 0 1000 1000;" xml:space="preserve">
<g>
	<path d="M876.8,323.2C831.5,278,769,250,700,250H550v100h150c82.8,0,150,67.2,150,150c0,41.4-16.8,78.9-43.9,106.1
		C778.9,633.2,741.4,650,700,650H550v100h150c138.1,0,250-111.9,250-250C950,431,922,368.5,876.8,323.2z M50,500
		c0,138.1,111.9,250,250,250h150V650H300c-82.8,0-150-67.2-150-150c0-41.4,16.8-78.9,43.9-106.1C221.1,366.8,258.6,350,300,350h150
		V250H300C161.9,250,50,361.9,50,500z"/>
	<path d="M250,500L250,500c0-27.6,22.4-50,50-50h400c27.6,0,50,22.4,50,50v0c0,27.6-22.4,50-50,50H300C272.4,550,250,527.6,250,500z
		"/>
</g>
</svg>)svg"},
        {"link-2.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Generator: Adobe Illustrator 23.0.0, SVG Export Plug-In . SVG Version: 6.00 Build 0)  -->
<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
	 viewBox="0 0 1000 1000" style="enable-background:new 0 0 1000 1000;" xml:space="preserve">
<path d="M1000.5,434.8v130.5c0,89.3-72.4,161.8-161.8,161.8H648.3l90.8-90.8h99.7c39.2,0,70.9-31.8,70.9-70.9V434.7
	c0-26.4-14.4-49.5-35.9-61.7c-10.3-5.9-22.3-9.3-35.1-9.3H526.6c-39.2,0-70.9,31.8-70.9,71v30.9h179.5v99.6
	c0,89.3-72.4,161.8-161.8,161.8H161.3c-2.2,0-4.4,0-6.6-0.1c-35.3-1.4-67.7-14.2-93.7-34.7c-24.3-19.2-43-45.3-53.1-75.4
	c-5.5-16.2-8.4-33.5-8.4-51.6V434.8c0-89.4,72.4-161.8,161.8-161.8h190.4l-90.8,90.8h-99.6c-39.2,0-71,31.8-71,71v130.5
	c0,26.4,14.4,49.5,35.9,61.7c10.3,5.9,22.3,9.3,35.1,9.3h312.1c39.2,0,71-31.8,71-70.9v-8.8H364.8V434.8
	c0-89.4,72.4-161.8,161.8-161.8h312.1c2.2,0,4.4,0,6.6,0.1c35.3,1.4,67.7,14.2,93.7,34.7c24.3,19.3,43,45.3,53.1,75.4
	C997.6,399.4,1000.5,416.7,1000.5,434.8z"/>
</svg>)svg"},
        {"Compare.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Generator: Adobe Illustrator 23.0.0, SVG Export Plug-In . SVG Version: 6.00 Build 0)  -->
<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
	 viewBox="0 0 1000 1000" style="enable-background:new 0 0 1000 1000;" xml:space="preserve">
<style type="text/css">
	.st0{fill:#FFFFFF;}
</style>
<g>
	<polygon points="79,776.5 79,223.5 387,223.5 387,144.5 0,144.5 0,855.5 387,855.5 387,776.5 	"/>
	<polygon points="1000,144.5 1000,855.5 613,855.5 613,540 781.9,540 781.9,613.1 895,500 781.9,386.9 781.9,460 613,460 613,144.5 
			"/>
	<rect x="460.5" width="79" height="1000"/>
	<polygon points="387,460 387,540 218.1,540 218.1,613.1 105,500 218.1,386.9 218.1,460 	"/>
	<polyline class="st0" points="613,460 781.9,460 781.9,386.9 895,500 781.9,613.1 781.9,540 613,540 	"/>
</g>
</svg>)svg"},
        {"Monitoring-1.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Generator: Adobe Illustrator 23.0.0, SVG Export Plug-In . SVG Version: 6.00 Build 0)  -->
<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
	 viewBox="0 0 1000 1000" style="enable-background:new 0 0 1000 1000;" xml:space="preserve">
<path d="M999.5,328.9v109.7h-80.1v-79.8c0-19.8-16-35.8-35.8-35.8H553.2L361.7,813.1h274.5v80.1H86.6c-47.6,0-86.1-38.5-86.1-86l0,0
	V192.8c0-47.5,38.5-86,86-86l0,0h328.1c16.4,0,32.5,6.5,42.8,19.2l0.1,0.1l93.4,115.5l1.1,1.4h361.5
	C961,242.9,999.5,281.4,999.5,328.9L999.5,328.9z"/>
<g>
	<path d="M837.7,668.7"/>
	<path d="M741.9,486.6c-142.6,0-258.1,128.6-258.1,128.6s115.6,128.6,258.1,128.6c142.6,0,258.1-128.6,258.1-128.6
		S884.4,486.6,741.9,486.6z M741.9,689.3c-82,0-148.5-74-148.5-74s66.5-74,148.5-74s148.5,74,148.5,74S823.9,689.3,741.9,689.3z"/>
	<circle cx="741.9" cy="615.3" r="49.9"/>
</g>
</svg>)svg"},
        {"Cross-Potent-1.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Generator: Adobe Illustrator 23.0.0, SVG Export Plug-In . SVG Version: 6.00 Build 0)  -->
<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
	 viewBox="0 0 1000 1000" style="enable-background:new 0 0 1000 1000;" xml:space="preserve">
<polygon points="750,450 750,350 850,350 850,650 750,650 750,550 250,550 250,650 150,650 150,350 250,350 250,450 "/>
<polygon points="550,750 650,750 650,850 350,850 350,750 450,750 450,250 350,250 350,150 650,150 650,250 550,250 "/>
</svg>)svg"},
        {"Crossbar-1.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Generator: Adobe Illustrator 23.0.0, SVG Export Plug-In . SVG Version: 6.00 Build 0)  -->
<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
	 viewBox="0 0 1000 1000" style="enable-background:new 0 0 1000 1000;" xml:space="preserve">
<polygon points="300,550 300,650 200,650 200,350 300,350 300,450 700,450 700,350 800,350 800,650 700,650 700,550 "/>
</svg>)svg"},
        {"Distribution-1.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Generator: Adobe Illustrator 23.0.0, SVG Export Plug-In . SVG Version: 6.00 Build 0)  -->
<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
	 viewBox="0 0 1000 1000" style="enable-background:new 0 0 1000 1000;" xml:space="preserve">
<g>
	<g>
		<path d="M1000,289.3L773.7,452.2c0,0-6.6-66.2-6.7-66.2c0.1,0.7-17.9,8-19.7,8.8c-6.6,3.2-13,6.6-19.4,10.3
			c-12.6,7.2-24.8,15.3-36.4,24.1c-23.1,17.6-43.9,38.2-61.7,61.1c-17.9,23.1-32.8,48.5-44.2,75.4c-11.6,27.6-19.4,56.7-23.2,86.4
			c-5.1,39.8-6.3,80.1-6.3,120.1c0,15.8,0,37.3,0,53.1H439.4c0-40.7-0.2-87.1-0.6-127.7c-0.4-31.4-3.2-62.3-11.6-92.6
			c-7.9-28.4-19.7-55.7-34.8-81c-14.9-25-33.1-47.9-53.9-68.2s-44.3-37.9-69.7-52.1c-11.3-6.3-22.9-12-34.8-17l-6.6,65.4L1,289.3
			l254.2-114.5l-8.8,88c107.6,35,197.5,109.1,253.1,205.5c55.9-96.9,146.6-171.3,255-206.1l-8.7-87.4L1000,289.3z"/>
	</g>
</g>
</svg>)svg"},
        {"Distribution-2.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Generator: Adobe Illustrator 23.0.0, SVG Export Plug-In . SVG Version: 6.00 Build 0)  -->
<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
	 viewBox="0 0 1000 1000" style="enable-background:new 0 0 1000 1000;" xml:space="preserve">
<g>
	<g>
		<g>
			<path d="M773.1,696.1l226.4,162.8L745.3,973.4l8.8-88c-107.6-35-197.5-109.1-253.1-205.5C445.1,776.8,354.4,851.2,246,886
				l8.7,87.4L0.5,858.9L226.8,696c0,0,6.5,65.2,6.7,66.2c0-0.7,17.9-8,19.7-8.8c6.6-3.2,13-6.6,19.4-10.3
				c12.6-7.2,24.8-15.3,36.4-24.1c23.1-17.6,43.9-38.2,61.7-61.1c17.9-23.1,32.8-48.5,44.2-75.4c14.2-33.4,24-69.7,17-106.1
				c-6.8-35.1-21.7-70-40-100.6c-14.9-25-33.1-47.9-53.9-68.2s-44.3-37.9-69.7-52.1c-11.3-6.3-22.9-12-34.8-17l-6.6,65.4L0.5,141.1
				L254.7,26.6l-8.8,88c107.6,35,197.5,109.1,253.1,205.5c55.9-96.9,146.6-171.3,255-206.1l-8.7-87.4l254.2,114.5L773.2,304
				c0,0-6.5-65.2-6.7-66.2c0,0.7-17.9,8-19.7,8.8c-6.6,3.2-13,6.6-19.4,10.3c-12.6,7.2-24.8,15.3-36.4,24.1
				c-23.1,17.6-43.9,38.2-61.7,61.1c-17.9,23.1-32.8,48.5-44.2,75.4c-8.1,19.1-16.7,39.4-19.6,60.5c-3,22.2,1.9,43.9,7.8,65.1
				c7.9,28.4,19.7,55.7,34.8,81c14.9,25,33.1,47.9,53.9,68.2c20.8,20.3,44.3,37.9,69.7,52.1c11.3,6.3,22.9,12,34.8,17L773.1,696.1z"
				/>
		</g>
	</g>
</g>
</svg>)svg"},
        {"Distribution-3.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Generator: Adobe Illustrator 23.0.0, SVG Export Plug-In . SVG Version: 6.00 Build 0)  -->
<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
	 viewBox="0 0 1000 1000" style="enable-background:new 0 0 1000 1000;" xml:space="preserve">
<g>
	<g>
		<path d="M999.5,465.2L773.2,628.1c0,0-6.5-65.2-6.7-66.2c0,0.7-17.9,8-19.7,8.8c-6.6,3.2-13,6.6-19.4,10.3
			c-12.6,7.2-24.8,15.3-36.4,24.1c-23.1,17.6-43.9,38.2-61.7,61.1c-17.9,23.1-32.8,48.5-44.2,75.4c-11.6,27.6-19.4,56.7-23.2,86.4
			c-5.1,39.8-6.3,80.1-6.3,120.1v53.1H438.9c0-40.7-0.2-87.1-0.6-127.7c-0.4-31.4-3.2-62.3-11.6-92.6c-7.9-28.4-19.7-55.7-34.8-81
			c-14.9-25-33.1-47.9-53.9-68.2s-44.3-37.9-69.7-52.1c-11.3-6.3-22.9-12-34.8-17l-6.6,65.4L0.5,465.2l254.2-114.5l-8.8,88
			c76.3,24.8,143.6,69.3,196.2,127.4V283.4H323.7L500,0l176.3,283.4H557.9v280.5c52.7-57.5,120-101.4,196.1-125.8l-8.7-87.4
			L999.5,465.2z"/>
	</g>
</g>
</svg>)svg"},
        {"PipeSymbol-1.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Generator: Adobe Illustrator 23.0.0, SVG Export Plug-In . SVG Version: 6.00 Build 0)  -->
<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
	 viewBox="0 0 1000 1000" style="enable-background:new 0 0 1000 1000;" xml:space="preserve">
<polygon points="450,300 350,300 350,200 650,200 650,300 550,300 550,700 650,700 650,800 350,800 350,700 450,700 "/>
</svg>)svg"},
        {"Scan-5.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Generator: Adobe Illustrator 23.0.0, SVG Export Plug-In . SVG Version: 6.00 Build 0)  -->
<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
	 viewBox="0 0 1000 1000" style="enable-background:new 0 0 1000 1000;" xml:space="preserve">
<polygon points="1000,0 1000,300 900,300 900,100 700,100 700,0 "/>
<g>
	<polygon points="300,0 300,100 100,100 100,300 0,300 0,0 	"/>
</g>
<polygon points="1000,700 1000,1000 700,1000 700,900 900,900 900,700 "/>
<polygon points="300,900 300,1000 0,1000 0,700 100,700 100,900 "/>
<g>
	<rect x="300" y="366.7" width="400" height="100"/>
	<rect x="200" y="533.3" width="600" height="100"/>
	<rect x="200" y="200.2" width="600" height="100"/>
	<rect x="300" y="699.8" width="400" height="100"/>
</g>
</svg>)svg"},
        {"Scan-6.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Generator: Adobe Illustrator 23.0.0, SVG Export Plug-In . SVG Version: 6.00 Build 0)  -->
<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
	 viewBox="0 0 1000 1000" style="enable-background:new 0 0 1000 1000;" xml:space="preserve">
<g>
	<polygon points="1000,0 1000,300 900,300 900,100 700,100 700,0 	"/>
	<g>
		<polygon points="300,0 300,100 100,100 100,300 0,300 0,0 		"/>
	</g>
	<polygon points="1000,700 1000,1000 700,1000 700,900 900,900 900,700 	"/>
	<polygon points="300,900 300,1000 0,1000 0,700 100,700 100,900 	"/>
	<g>
		<rect x="300" y="450" width="400" height="100"/>
		<rect x="200" y="650" width="600" height="100"/>
		<rect x="200" y="250" width="600" height="100"/>
	</g>
</g>
</svg>)svg"},
        {"Scan-7.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Generator: Adobe Illustrator 23.0.0, SVG Export Plug-In . SVG Version: 6.00 Build 0)  -->
<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
	 viewBox="0 0 1000 1000" style="enable-background:new 0 0 1000 1000;" xml:space="preserve">
<g>
	<polygon points="1000,0 1000,300 900,300 900,100 700,100 700,0 	"/>
	<g>
		<polygon points="300,0 300,100 100,100 100,300 0,300 0,0 		"/>
	</g>
	<polygon points="1000,700 1000,1000 700,1000 700,900 900,900 900,700 	"/>
	<polygon points="300,900 300,1000 0,1000 0,700 100,700 100,900 	"/>
	<rect x="200" y="450" width="600" height="100"/>
	<rect x="300" y="650" width="400" height="100"/>
	<rect x="300" y="250" width="400" height="100"/>
</g>
</svg>)svg"},
        {"link-3.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Generator: Adobe Illustrator 23.0.0, SVG Export Plug-In . SVG Version: 6.00 Build 0)  -->
<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
	 viewBox="0 0 1000 1000" style="enable-background:new 0 0 1000 1000;" xml:space="preserve">
<path d="M1000.5,434.8v130.5c0,89.3-72.4,161.8-161.8,161.8H648.3l90.8-90.8h99.7c39.2,0,70.9-31.8,70.9-70.9V434.7
	c0-26.4-14.4-49.5-35.9-61.7c-10.3-5.9-22.3-9.3-35.1-9.3H526.6c-39.2,0-70.9,31.8-70.9,71v30.9h179.5v99.6
	c0,89.3-72.4,161.8-161.8,161.8H161.3c-2.2,0-4.4,0-6.6-0.1c-35.3-1.4-67.7-14.2-93.7-34.7c-24.3-19.2-43-45.3-53.1-75.4
	c-5.5-16.2-8.4-33.5-8.4-51.6V434.8c0-89.4,72.4-161.8,161.8-161.8h190.4l-90.8,90.8h-99.6c-39.2,0-71,31.8-71,71v130.5
	c0,26.4,14.4,49.5,35.9,61.7c10.3,5.9,22.3,9.3,35.1,9.3h312.1c39.2,0,71-31.8,71-70.9v-8.8H364.8V434.8
	c0-89.4,72.4-161.8,161.8-161.8h312.1c2.2,0,4.4,0,6.6,0.1c35.3,1.4,67.7,14.2,93.7,34.7c24.3,19.3,43,45.3,53.1,75.4
	C997.6,399.4,1000.5,416.7,1000.5,434.8z"/>
</svg>)svg"},
        {"link-4.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Generator: Adobe Illustrator 23.0.0, SVG Export Plug-In . SVG Version: 6.00 Build 0)  -->
<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
	 viewBox="0 0 1000 1000" style="enable-background:new 0 0 1000 1000;" xml:space="preserve">
<g>
	<path d="M876.8,323.2C831.5,278,769,250,700,250H550v100h150c82.8,0,150,67.2,150,150c0,41.4-16.8,78.9-43.9,106.1
		C778.9,633.2,741.4,650,700,650H550v100h150c138.1,0,250-111.9,250-250C950,431,922,368.5,876.8,323.2z M50,500
		c0,138.1,111.9,250,250,250h150V650H300c-82.8,0-150-67.2-150-150c0-41.4,16.8-78.9,43.9-106.1C221.1,366.8,258.6,350,300,350h150
		V250H300C161.9,250,50,361.9,50,500z"/>
	<path d="M250,500L250,500c0-27.6,22.4-50,50-50h400c27.6,0,50,22.4,50,50v0c0,27.6-22.4,50-50,50H300C272.4,550,250,527.6,250,500z
		"/>
</g>
</svg>)svg"},
        {"path--3.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Generator: Adobe Illustrator 23.0.0, SVG Export Plug-In . SVG Version: 6.00 Build 0)  -->
<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
	 viewBox="0 0 1000 1000" style="enable-background:new 0 0 1000 1000;" xml:space="preserve">
<g>
	<polygon points="372.2,600 0,600 0,400 315.8,400 320.7,417.3 325,432.6 344,500 347.8,513.6 	"/>
	<polygon points="1000,400 1000,600 684.2,600 679.3,582.7 675,567.4 656,500 652.2,486.4 627.8,400 	"/>
	<polygon points="629.5,775.2 625.5,776.3 526,776.3 506.2,706.1 479,609.8 476.3,600 471,581.2 452,513.6 448,500 424.8,417.3
		420,400 397.7,321 370.5,224.8 374.5,223.7 474.2,223.7 482.4,252.9 494,293.9 501.6,321 503.6,328.2 521,390.2 523.8,400
		543.2,468.9 548.2,486.4 549.5,491.4 552,500 571,567.4 575.3,582.7 580.2,600 602.4,679 	"/>
</g>
</svg>)svg"},
        {"path-2.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Generator: Adobe Illustrator 23.0.0, SVG Export Plug-In . SVG Version: 6.00 Build 0)  -->
<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
	 viewBox="0 0 1000 1000" style="enable-background:new 0 0 1000 1000;" xml:space="preserve">
<style type="text/css">
	.st0{fill:none;}
</style>
<g>
	<polygon points="655.3,600 401,600 396.2,582.7 372.9,500 369,486.4 350,418.8 344.7,400 599,400 603.8,417.3 627.1,500 631,513.6
		650,581.2 	"/>
</g>
<g>
	<polygon points="197.2,600 0,600 0,400 140.8,400 145.7,417.3 150,432.6 169,500 172.8,513.6 	"/>
	<polygon points="1000,400 1000,600 859.2,600 854.3,582.7 850,567.4 831,500 827.2,486.4 802.8,400 	"/>
</g>
<g>
	<g>
		<polygon points="400.4,775.2 396.5,776.3 296.8,776.3 277,706.1 277,706.1 249.9,609.8 247.2,600 222.8,513.6 219,500 200,432.6
			195.7,417.3 190.8,400 168.6,321 141.5,224.8 145.4,223.7 245,223.7 264.8,293.9 264.8,293.9 291.9,390.2 294.7,400 300,418.8
			319,486.4 322.9,500 346.2,582.7 351,600 373.3,679 		"/>
	</g>
	<g>
		<polygon points="858.5,775.2 854.6,776.3 755,776.3 735.2,706.1 735.2,706.1 708.1,609.8 705.3,600 700,581.2 681,513.6
			677.1,500 653.8,417.3 649,400 626.7,321 599.6,224.8 603.5,223.7 703.2,223.7 723,293.9 723,293.9 750.1,390.2 752.8,400
			777.2,486.4 781,500 800,567.4 804.3,582.7 809.2,600 831.4,679 		"/>
	</g>
</g>
<line class="st0" x1="0" y1="223.7" x2="6.6" y2="223.7"/>
<line class="st0" x1="0" y1="223.7" x2="16.7" y2="223.7"/>
<line class="st0" x1="0" y1="223.7" x2="35.8" y2="223.7"/>
<line class="st0" x1="0" y1="223.7" x2="56" y2="223.7"/>
<line class="st0" x1="0" y1="223.7" x2="71.8" y2="223.7"/>
<line class="st0" x1="0" y1="223.7" x2="87.5" y2="223.7"/>
<line class="st0" x1="0" y1="223.7" x2="104.4" y2="223.7"/>
<line class="st0" x1="0" y1="223.7" x2="126.8" y2="223.7"/>
<polyline class="st0" points="174,223.7 145.4,223.7 126.8,223.7 104.4,223.7 87.5,223.7 71.8,223.7 56,223.7 0,223.7 "/>
<polyline class="st0" points="232.4,223.7 174,223.7 145.4,223.7 126.8,223.7 104.4,223.7 87.5,223.7 71.8,223.7 56,223.7 0,223.7
	"/>
<polyline class="st0" points="290.9,223.7 245,223.7 232.4,223.7 174,223.7 145.4,223.7 126.8,223.7 104.4,223.7 87.5,223.7
	71.8,223.7 56,223.7 0,223.7 "/>
<polyline class="st0" points="349.3,223.7 290.9,223.7 245,223.7 232.4,223.7 174,223.7 145.4,223.7 126.8,223.7 104.4,223.7
	87.5,223.7 71.8,223.7 56,223.7 0,223.7 "/>
<polyline class="st0" points="381.9,223.7 349.3,223.7 290.9,223.7 245,223.7 232.4,223.7 174,223.7 145.4,223.7 126.8,223.7
	104.4,223.7 87.5,223.7 71.8,223.7 56,223.7 0,223.7 "/>
<polyline class="st0" points="411.1,223.7 381.9,223.7 349.3,223.7 290.9,223.7 245,223.7 232.4,223.7 174,223.7 145.4,223.7
	126.8,223.7 104.4,223.7 87.5,223.7 71.8,223.7 56,223.7 0,223.7 "/>
<polyline class="st0" points="436.9,223.7 411.1,223.7 381.9,223.7 349.3,223.7 290.9,223.7 245,223.7 232.4,223.7 174,223.7
	145.4,223.7 126.8,223.7 104.4,223.7 87.5,223.7 71.8,223.7 56,223.7 0,223.7 "/>
<polyline class="st0" points="467.3,223.7 436.9,223.7 411.1,223.7 381.9,223.7 349.3,223.7 290.9,223.7 245,223.7 232.4,223.7
	174,223.7 145.4,223.7 126.8,223.7 104.4,223.7 87.5,223.7 71.8,223.7 56,223.7 0,223.7 "/>
<polyline class="st0" points="500,223.7 467.3,223.7 436.9,223.7 411.1,223.7 381.9,223.7 349.3,223.7 290.9,223.7 245,223.7
	232.4,223.7 174,223.7 145.4,223.7 126.8,223.7 104.4,223.7 87.5,223.7 71.8,223.7 56,223.7 0,223.7 "/>
<polyline class="st0" points="534.7,223.7 500,223.7 467.3,223.7 436.9,223.7 411.1,223.7 381.9,223.7 349.3,223.7 290.9,223.7
	245,223.7 232.4,223.7 174,223.7 145.4,223.7 126.8,223.7 104.4,223.7 87.5,223.7 71.8,223.7 56,223.7 0,223.7 "/>
<polyline class="st0" points="566.2,223.7 534.7,223.7 500,223.7 467.3,223.7 436.9,223.7 411.1,223.7 381.9,223.7 349.3,223.7
	290.9,223.7 245,223.7 232.4,223.7 174,223.7 145.4,223.7 126.8,223.7 104.4,223.7 87.5,223.7 71.8,223.7 56,223.7 0,223.7 "/>
<polyline class="st0" points="592,223.7 566.2,223.7 534.7,223.7 500,223.7 467.3,223.7 436.9,223.7 411.1,223.7 381.9,223.7
	349.3,223.7 290.9,223.7 245,223.7 232.4,223.7 174,223.7 145.4,223.7 126.8,223.7 104.4,223.7 87.5,223.7 71.8,223.7 56,223.7
	0,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 957.2,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 957.2,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 957.2,223.7
	958.3,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 957.2,223.7
	958.3,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 957.2,223.7
	958.3,223.7 962.8,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 957.2,223.7
	958.3,223.7 962.8,223.7 965,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 957.2,223.7
	958.3,223.7 962.8,223.7 965,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 957.2,223.7
	958.3,223.7 962.8,223.7 965,223.7 966.2,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 957.2,223.7
	958.3,223.7 962.8,223.7 965,223.7 966.2,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 957.2,223.7
	958.3,223.7 962.8,223.7 965,223.7 966.2,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 957.2,223.7
	958.3,223.7 962.8,223.7 965,223.7 966.2,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 957.2,223.7
	958.3,223.7 962.8,223.7 965,223.7 966.2,223.7 968.4,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 957.2,223.7
	958.3,223.7 962.8,223.7 965,223.7 966.2,223.7 968.4,223.7 974,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 957.2,223.7
	958.3,223.7 962.8,223.7 965,223.7 966.2,223.7 968.4,223.7 974,223.7 980.8,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 957.2,223.7
	958.3,223.7 962.8,223.7 965,223.7 966.2,223.7 968.4,223.7 974,223.7 980.8,223.7 984.1,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 957.2,223.7
	958.3,223.7 962.8,223.7 965,223.7 966.2,223.7 968.4,223.7 974,223.7 980.8,223.7 984.1,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 957.2,223.7
	958.3,223.7 962.8,223.7 965,223.7 966.2,223.7 968.4,223.7 974,223.7 980.8,223.7 984.1,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 957.2,223.7
	958.3,223.7 962.8,223.7 965,223.7 966.2,223.7 968.4,223.7 974,223.7 980.8,223.7 984.1,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 957.2,223.7
	958.3,223.7 962.8,223.7 965,223.7 966.2,223.7 968.4,223.7 974,223.7 980.8,223.7 984.1,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 957.2,223.7
	958.3,223.7 962.8,223.7 965,223.7 966.2,223.7 968.4,223.7 974,223.7 980.8,223.7 984.1,223.7 986.4,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 957.2,223.7
	958.3,223.7 962.8,223.7 965,223.7 966.2,223.7 968.4,223.7 974,223.7 980.8,223.7 984.1,223.7 986.4,223.7 988.6,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 957.2,223.7
	958.3,223.7 962.8,223.7 965,223.7 966.2,223.7 968.4,223.7 974,223.7 980.8,223.7 984.1,223.7 986.4,223.7 988.6,223.7
	990.9,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 957.2,223.7
	958.3,223.7 962.8,223.7 965,223.7 966.2,223.7 968.4,223.7 974,223.7 980.8,223.7 984.1,223.7 986.4,223.7 988.6,223.7
	990.9,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 957.2,223.7
	958.3,223.7 962.8,223.7 965,223.7 966.2,223.7 968.4,223.7 974,223.7 980.8,223.7 984.1,223.7 986.4,223.7 988.6,223.7
	990.9,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 957.2,223.7
	958.3,223.7 962.8,223.7 965,223.7 966.2,223.7 968.4,223.7 974,223.7 980.8,223.7 984.1,223.7 986.4,223.7 988.6,223.7
	990.9,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 957.2,223.7
	958.3,223.7 962.8,223.7 965,223.7 966.2,223.7 968.4,223.7 974,223.7 980.8,223.7 984.1,223.7 986.4,223.7 988.6,223.7
	990.9,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 957.2,223.7
	958.3,223.7 962.8,223.7 965,223.7 966.2,223.7 968.4,223.7 974,223.7 980.8,223.7 984.1,223.7 986.4,223.7 988.6,223.7
	990.9,223.7 992,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 957.2,223.7
	958.3,223.7 962.8,223.7 965,223.7 966.2,223.7 968.4,223.7 974,223.7 980.8,223.7 984.1,223.7 986.4,223.7 988.6,223.7
	990.9,223.7 992,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 957.2,223.7
	958.3,223.7 962.8,223.7 965,223.7 966.2,223.7 968.4,223.7 974,223.7 980.8,223.7 984.1,223.7 986.4,223.7 988.6,223.7
	990.9,223.7 992,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 957.2,223.7
	958.3,223.7 962.8,223.7 965,223.7 966.2,223.7 968.4,223.7 974,223.7 980.8,223.7 984.1,223.7 986.4,223.7 988.6,223.7
	990.9,223.7 992,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 957.2,223.7
	958.3,223.7 962.8,223.7 965,223.7 966.2,223.7 968.4,223.7 974,223.7 980.8,223.7 984.1,223.7 986.4,223.7 988.6,223.7
	990.9,223.7 992,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 957.2,223.7
	958.3,223.7 962.8,223.7 965,223.7 966.2,223.7 968.4,223.7 974,223.7 980.8,223.7 984.1,223.7 986.4,223.7 988.6,223.7
	990.9,223.7 992,223.7 995.4,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 957.2,223.7
	958.3,223.7 962.8,223.7 965,223.7 966.2,223.7 968.4,223.7 974,223.7 980.8,223.7 984.1,223.7 986.4,223.7 988.6,223.7
	990.9,223.7 992,223.7 995.4,223.7 1000,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 957.2,223.7
	958.3,223.7 962.8,223.7 965,223.7 966.2,223.7 968.4,223.7 974,223.7 980.8,223.7 984.1,223.7 986.4,223.7 988.6,223.7
	990.9,223.7 992,223.7 995.4,223.7 1000,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 957.2,223.7
	958.3,223.7 962.8,223.7 965,223.7 966.2,223.7 968.4,223.7 974,223.7 980.8,223.7 984.1,223.7 986.4,223.7 988.6,223.7
	990.9,223.7 992,223.7 995.4,223.7 1000,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 957.2,223.7
	958.3,223.7 962.8,223.7 965,223.7 966.2,223.7 968.4,223.7 974,223.7 980.8,223.7 984.1,223.7 986.4,223.7 988.6,223.7
	990.9,223.7 992,223.7 995.4,223.7 1000,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 957.2,223.7
	958.3,223.7 962.8,223.7 965,223.7 966.2,223.7 968.4,223.7 974,223.7 980.8,223.7 984.1,223.7 986.4,223.7 988.6,223.7
	990.9,223.7 992,223.7 995.4,223.7 1000,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 957.2,223.7
	958.3,223.7 962.8,223.7 965,223.7 966.2,223.7 968.4,223.7 974,223.7 980.8,223.7 984.1,223.7 986.4,223.7 988.6,223.7
	990.9,223.7 992,223.7 995.4,223.7 1000,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 957.2,223.7
	958.3,223.7 962.8,223.7 965,223.7 966.2,223.7 968.4,223.7 974,223.7 980.8,223.7 984.1,223.7 986.4,223.7 988.6,223.7
	990.9,223.7 992,223.7 995.4,223.7 1000,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 957.2,223.7
	958.3,223.7 962.8,223.7 965,223.7 966.2,223.7 968.4,223.7 974,223.7 980.8,223.7 984.1,223.7 986.4,223.7 988.6,223.7
	990.9,223.7 992,223.7 995.4,223.7 1000,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 957.2,223.7
	958.3,223.7 962.8,223.7 965,223.7 966.2,223.7 968.4,223.7 974,223.7 980.8,223.7 984.1,223.7 986.4,223.7 988.6,223.7
	990.9,223.7 992,223.7 995.4,223.7 1000,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 957.2,223.7
	958.3,223.7 962.8,223.7 965,223.7 966.2,223.7 968.4,223.7 974,223.7 980.8,223.7 984.1,223.7 986.4,223.7 988.6,223.7
	990.9,223.7 992,223.7 995.4,223.7 1000,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 957.2,223.7
	958.3,223.7 962.8,223.7 965,223.7 966.2,223.7 968.4,223.7 974,223.7 980.8,223.7 984.1,223.7 986.4,223.7 988.6,223.7
	990.9,223.7 992,223.7 995.4,223.7 1000,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 957.2,223.7
	958.3,223.7 962.8,223.7 965,223.7 966.2,223.7 968.4,223.7 974,223.7 980.8,223.7 984.1,223.7 986.4,223.7 988.6,223.7
	990.9,223.7 992,223.7 995.4,223.7 1000,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 957.2,223.7
	958.3,223.7 962.8,223.7 965,223.7 966.2,223.7 968.4,223.7 974,223.7 980.8,223.7 984.1,223.7 986.4,223.7 988.6,223.7
	990.9,223.7 992,223.7 995.4,223.7 1000,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 957.2,223.7
	958.3,223.7 962.8,223.7 965,223.7 966.2,223.7 968.4,223.7 974,223.7 980.8,223.7 984.1,223.7 986.4,223.7 988.6,223.7
	990.9,223.7 992,223.7 995.4,223.7 1000,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 957.2,223.7
	958.3,223.7 962.8,223.7 965,223.7 966.2,223.7 968.4,223.7 974,223.7 980.8,223.7 984.1,223.7 986.4,223.7 988.6,223.7
	990.9,223.7 992,223.7 995.4,223.7 1000,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 957.2,223.7
	958.3,223.7 962.8,223.7 965,223.7 966.2,223.7 968.4,223.7 974,223.7 980.8,223.7 984.1,223.7 986.4,223.7 988.6,223.7
	990.9,223.7 992,223.7 995.4,223.7 1000,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 957.2,223.7
	958.3,223.7 962.8,223.7 965,223.7 966.2,223.7 968.4,223.7 974,223.7 980.8,223.7 984.1,223.7 986.4,223.7 988.6,223.7
	990.9,223.7 992,223.7 995.4,223.7 1000,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 957.2,223.7
	958.3,223.7 962.8,223.7 965,223.7 966.2,223.7 968.4,223.7 974,223.7 980.8,223.7 984.1,223.7 986.4,223.7 988.6,223.7
	990.9,223.7 992,223.7 995.4,223.7 1000,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 957.2,223.7
	958.3,223.7 962.8,223.7 965,223.7 966.2,223.7 968.4,223.7 974,223.7 980.8,223.7 984.1,223.7 986.4,223.7 988.6,223.7
	990.9,223.7 992,223.7 995.4,223.7 1000,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 957.2,223.7
	958.3,223.7 962.8,223.7 965,223.7 966.2,223.7 968.4,223.7 974,223.7 980.8,223.7 984.1,223.7 986.4,223.7 988.6,223.7
	990.9,223.7 992,223.7 995.4,223.7 1000,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 957.2,223.7
	958.3,223.7 962.8,223.7 965,223.7 966.2,223.7 968.4,223.7 974,223.7 980.8,223.7 984.1,223.7 986.4,223.7 988.6,223.7
	990.9,223.7 992,223.7 995.4,223.7 1000,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 957.2,223.7
	958.3,223.7 962.8,223.7 965,223.7 966.2,223.7 968.4,223.7 974,223.7 980.8,223.7 984.1,223.7 986.4,223.7 988.6,223.7
	990.9,223.7 992,223.7 995.4,223.7 1000,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 957.2,223.7
	958.3,223.7 962.8,223.7 965,223.7 966.2,223.7 968.4,223.7 974,223.7 980.8,223.7 984.1,223.7 986.4,223.7 988.6,223.7
	990.9,223.7 992,223.7 995.4,223.7 1000,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 957.2,223.7
	958.3,223.7 962.8,223.7 965,223.7 966.2,223.7 968.4,223.7 974,223.7 980.8,223.7 984.1,223.7 986.4,223.7 988.6,223.7
	990.9,223.7 992,223.7 995.4,223.7 1000,223.7 "/>
<polyline class="st0" points="0,223.7 56,223.7 71.8,223.7 87.5,223.7 104.4,223.7 126.8,223.7 145.4,223.7 174,223.7 232.4,223.7
	245,223.7 290.9,223.7 349.3,223.7 381.9,223.7 411.1,223.7 436.9,223.7 467.3,223.7 500,223.7 534.7,223.7 566.2,223.7 592,223.7
	603.5,223.7 619,223.7 634.7,223.7 661.7,223.7 681.9,223.7 703.2,223.7 704.4,223.7 722.3,223.7 738.1,223.7 758.3,223.7
	772.9,223.7 787.5,223.7 797.6,223.7 807.7,223.7 819,223.7 831.3,223.7 850.4,223.7 858.5,223.7 876.3,223.7 888.6,223.7
	901.4,223.7 908.8,223.7 916.7,223.7 925.7,223.7 933.6,223.7 938.1,223.7 941.4,223.7 947.1,223.7 951.5,223.7 957.2,223.7
	958.3,223.7 962.8,223.7 965,223.7 966.2,223.7 968.4,223.7 974,223.7 980.8,223.7 984.1,223.7 986.4,223.7 988.6,223.7
	990.9,223.7 992,223.7 995.4,223.7 1000,223.7 "/>
</svg>)svg"},
        {"path-3.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Generator: Adobe Illustrator 23.0.0, SVG Export Plug-In . SVG Version: 6.00 Build 0)  -->
<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
	 viewBox="0 0 1000 1000" style="enable-background:new 0 0 1000 1000;" xml:space="preserve">
<path d="M243.9,393.9C216.8,421.1,200,458.6,200,500c0,82.8,67.2,150,150,150h100v100H350c-138.1,0-250-111.9-250-250
	s111.9-250,250-250h100v100H350C308.6,350,271.1,366.8,243.9,393.9z"/>
<path d="M900,500c0,138.1-111.9,250-250,250H550V650h100c41.4,0,78.9-16.8,106.1-43.9C783.2,578.9,800,541.4,800,500
	c0-82.8-67.2-150-150-150H550V250h100c69,0,131.5,28,176.8,73.2C872,368.5,900,431,900,500z"/>
<path d="M300,500L300,500c0-27.6,22.4-50,50-50h300c27.6,0,50,22.4,50,50v0c0,27.6-22.4,50-50,50H350C322.4,550,300,527.6,300,500z"
	/>
</svg>)svg"},
        {"save-thumbnails-1.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Generator: Adobe Illustrator 23.0.0, SVG Export Plug-In . SVG Version: 6.00 Build 0)  -->
<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
	 viewBox="0 0 1000 1000" style="enable-background:new 0 0 1000 1000;" xml:space="preserve">
<style type="text/css">
	.st0{fill-rule:evenodd;clip-rule:evenodd;}
</style>
<title>image-picture</title>
<desc>Created with Sketch Beta.</desc>
<g>
	<path d="M552,242.6v0.3l-1.1-1.4C551.3,241.9,551.7,242.3,552,242.6z"/>
	<path class="st0" d="M551.5,404.4c0,22.5-18.2,40.7-40.7,40.7c-1.7,0-3.4-0.1-5.1-0.3c-20.1-2.5-35.6-19.7-35.6-40.4
		c0-22.5,18.3-40.7,40.7-40.7c8.8,0,17,2.8,23.6,7.6C544.7,378.6,551.5,390.8,551.5,404.4z"/>
	<path class="st0" d="M349.2,254.4c12.5-7.3,27.1-11.5,42.6-11.5h160.1l-1-1.3l-0.1-0.1l-93.3-115.4l-0.1-0.1
		c-10.3-12.7-26.4-19.2-42.8-19.2H86.5c-47.5,0-86,38.5-86,86v614.4c0,47.5,38.5,86,86.1,86h305.2c-1.1,0-2.2,0-3.3-0.1
		c-45.3-1.7-81.5-39-81.5-84.7V327.6C307.1,296.4,324,269.1,349.2,254.4z"/>
	<path d="M957.4,254.9c-12.8-7.6-27.8-12-43.9-11.9H552l36,40.6H395.6c-26.4,0-47.8,21.4-47.8,47.8v473.2
		c0,26.4,21.4,47.8,47.8,47.8h-3.3l-3.8,40.7c1.1,0.1,2.2,0.1,3.3,0.1H918c14.3,0,27.7-3.7,39.3-10.1c25.1-13.9,42.1-40.6,42.1-71.3
		V328.9C999.5,297.4,982.6,269.9,957.4,254.9z M510.7,323c14,0,27.2,3.5,38.7,9.8c25.5,13.8,42.8,40.7,42.8,71.7
		c0,45-36.5,81.5-81.5,81.5c-7,0-13.7-0.9-20.2-2.5c-35.2-9-61.3-40.9-61.3-78.9C429.3,359.4,465.7,323,510.7,323z M429.3,852.5
		c-22,0-40-17.5-40.7-39.4c0-0.4,0-0.9,0-1.4v-19.1l161.8-144.9l204.8,204.8H429.3z M958.8,811.7c0,22.5-18.3,40.7-40.7,40.7H812.8
		l-150.1-152l173.8-173.8l122.2,122.2V811.7z"/>
	<path d="M391.8,893.2h-3.3l0-0.1C389.6,893.2,390.7,893.2,391.8,893.2z"/>
</g>
</svg>)svg"},
        {"save-thumbnails-2.svg", R"svg(<?xml version="1.0" encoding="utf-8"?>
<!-- Generator: Adobe Illustrator 23.0.0, SVG Export Plug-In . SVG Version: 6.00 Build 0)  -->
<svg fill="currentColor" version="1.1" id="图层_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
	 viewBox="0 0 1000 1000" style="enable-background:new 0 0 1000 1000;" xml:space="preserve">
<style type="text/css">
	.st0{fill-rule:evenodd;clip-rule:evenodd;}
</style>
<g>
	<circle cx="552.9" cy="445.9" r="30.5"/>
	<path d="M1000,328.7v109.7h-40.6V324c0-22.5-18.2-40.7-40.7-40.7H431c-22.5,0-40.7,18.2-40.7,40.7v487.7
		c0,22.5,18.2,40.7,40.7,40.7h205.7V893H87.1C39.5,893,1,854.5,1,807V192.6c0-47.5,38.5-86,86-86h328.1c16.4,0,32.5,6.5,42.8,19.2
		l0.1,0.1l93.4,115.5l1,1.3l0.1,0.1H914C961.5,242.7,1000,281.2,1000,328.7z"/>
	<path d="M552.5,242.4v0.3l-1.1-1.4C551.8,241.7,552.2,242.1,552.5,242.4z"/>
	<g id="Page-1">
		<g id="Icon-Set-Filled" transform="translate(-362.000000, -101.000000)">
			<path id="image-picture" class="st0" d="M914.9,516.4c-16.8,0-30.5,13.6-30.5,30.5s13.7,30.5,30.5,30.5
				c16.8,0,30.5-13.6,30.5-30.5S931.8,516.4,914.9,516.4z M914.9,516.4c-16.8,0-30.5,13.6-30.5,30.5s13.7,30.5,30.5,30.5
				c16.8,0,30.5-13.6,30.5-30.5S931.8,516.4,914.9,516.4z M914.9,516.4c-16.8,0-30.5,13.6-30.5,30.5s13.7,30.5,30.5,30.5
				c16.8,0,30.5-13.6,30.5-30.5S931.8,516.4,914.9,516.4z M914.9,516.4c-16.8,0-30.5,13.6-30.5,30.5s13.7,30.5,30.5,30.5
				c16.8,0,30.5-13.6,30.5-30.5S931.8,516.4,914.9,516.4z M914.9,516.4c-16.8,0-30.5,13.6-30.5,30.5s13.7,30.5,30.5,30.5
				c16.8,0,30.5-13.6,30.5-30.5S931.8,516.4,914.9,516.4z M914.9,516.4c-16.8,0-30.5,13.6-30.5,30.5s13.7,30.5,30.5,30.5
				c16.8,0,30.5-13.6,30.5-30.5S931.8,516.4,914.9,516.4z M914.9,516.4c-16.8,0-30.5,13.6-30.5,30.5s13.7,30.5,30.5,30.5
				c16.8,0,30.5-13.6,30.5-30.5S931.8,516.4,914.9,516.4z M914.9,516.4c-16.8,0-30.5,13.6-30.5,30.5s13.7,30.5,30.5,30.5
				c16.8,0,30.5-13.6,30.5-30.5S931.8,516.4,914.9,516.4z M914.9,516.4c-16.8,0-30.5,13.6-30.5,30.5s13.7,30.5,30.5,30.5
				c16.8,0,30.5-13.6,30.5-30.5S931.8,516.4,914.9,516.4z"/>
		</g>
	</g>
	<g>
		<g>
			<g>
				<path class="st0" d="M917.3,242.7H432.4c-45.7,0-82.7,37-82.7,82.7v484.9c0,45.7,37,82.7,82.7,82.7h484.9
					c45.7,0,82.7-37,82.7-82.7V325.4C1000,279.7,963,242.7,917.3,242.7z M959.4,811.7c0,22.5-18.2,40.7-40.7,40.7H431
					c-22.5,0-40.7-18.2-40.7-40.7V324c0-22.5,18.2-40.7,40.7-40.7h487.7c22.5,0,40.7,18.2,40.7,40.7V811.7z"/>
				<path class="st0" d="M857.8,324H492c-33.7,0-61,27.3-61,61v365.8c0,33.7,27.3,61,61,61h365.8c33.7,0,61-27.3,61-61V385
					C918.7,351.3,891.4,324,857.8,324z M552.9,385c33.7,0,61,27.3,61,61s-27.3,61-61,61c-33.7,0-61-27.3-61-61S519.3,385,552.9,385z
					 M492,781.2c-16.8,0-30.5-13.7-30.5-30.5v-14.3L582.6,628l13.1,13.1l140.1,140.1H492z M888.2,750.8c0,16.8-13.7,30.5-30.5,30.5
					H779L666.7,667.5l130.1-130.1l91.4,91.5V750.8z"/>
			</g>
		</g>
	</g>
</g>
</svg>)svg"},
    };
}

#endif // SVGICONS_H

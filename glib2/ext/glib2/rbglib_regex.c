/*
 *  Copyright (C) 2015-2016  Ruby-GNOME2 Project Team
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *  MA  02110-1301  USA
 */

#include "rbgprivate.h"


#define RG_TARGET_NAMESPACE cRegex
#define _SELF(s) ((GRegex*)RVAL2BOXED(s, G_TYPE_REGEX))

static VALUE
rg_initialize(gint argc, VALUE *argv, VALUE self)
{
    GError *error = NULL;
    GRegex *regex = NULL;

    VALUE rb_pattern, rb_compile_options, rb_match_options;
    VALUE rb_options;
    const char *pattern;
    GRegexCompileFlags compile_options = 0;
    GRegexMatchFlags match_options = 0;

    rb_scan_args(argc, argv, "11", &rb_pattern, &rb_options);
    rbg_scan_options(rb_options,
                     "compile_options", &rb_compile_options,
                     "match_options", &rb_match_options,
                     NULL);

    pattern = RVAL2CSTR(rb_pattern);
    if (!NIL_P(rb_compile_options))
        compile_options = RVAL2GREGEXCOMPILEOPTIONSFLAGS(rb_compile_options);
    if (!NIL_P(rb_match_options))
        match_options = RVAL2GREGEXMATCHOPTIONSFLAGS(rb_match_options);

    regex = g_regex_new(pattern,
                        compile_options,
                        match_options,
                        &error);
    if (error)
        RAISE_GERROR(error);

    G_INITIALIZE(self, regex);
    return Qnil;
}

static VALUE
rg_pattern(VALUE self)
{
    return CSTR2RVAL(g_regex_get_pattern(_SELF(self)));
}

static VALUE
rg_compile_flags(VALUE self)
{
    return UINT2NUM(g_regex_get_compile_flags(_SELF(self)));
}

static VALUE
rg_match_flags(VALUE self)
{
    return UINT2NUM(g_regex_get_match_flags(_SELF(self)));
}

static VALUE
rg_split(gint argc, VALUE *argv, VALUE self)
{
    VALUE rb_string, rb_start_position, rb_match_options, rb_max_tokens, rb_options;
    GError *error = NULL;
    gchar **strings;
    const gchar *string;
    gssize string_len = -1;
    gint start_position = 0;
    GRegexMatchFlags match_options = 0;
    gint max_tokens = 0;

    rb_scan_args(argc, argv, "11", &rb_string, &rb_options);

    rbg_scan_options(rb_options,
                     "start_position", &rb_start_position,
                     "match_options", &rb_match_options,
                     "max_tokens", &rb_max_tokens,
                     NULL);
    string = RVAL2CSTR(rb_string);
    string_len = RSTRING_LEN(rb_string);

    if (!NIL_P(rb_start_position))
        start_position = NUM2INT(rb_start_position);
    if (!NIL_P(rb_match_options))
        match_options = RVAL2GREGEXMATCHOPTIONSFLAGS(rb_match_options);
    if (!NIL_P(rb_max_tokens))
        max_tokens = NUM2INT(rb_max_tokens);

    strings = g_regex_split_full(_SELF(self),
                                 string,
                                 string_len,
                                 start_position,
                                 match_options,
                                 max_tokens,
                                 &error);

    if (error)
        RAISE_GERROR(error);

    return STRV2RVAL_FREE(strings);
}

static VALUE
rg_match(gint argc, VALUE *argv, VALUE self)
{
    VALUE rb_string, rb_start_position, rb_match_options, rb_options;
    VALUE rb_frozen_string, rb_match_info;
    GMatchInfo *match_info = NULL;
    GError *error = NULL;
    const gchar *string;
    gssize string_len = -1;
    gint start_position = 0;
    GRegexMatchFlags match_options = 0;

    rb_scan_args(argc, argv, "11", &rb_string, &rb_options);

    rbg_scan_options(rb_options,
                     "start_position", &rb_start_position,
                     "match_options", &rb_match_options,
                     NULL);

    if (OBJ_FROZEN(rb_string)) {
        rb_frozen_string = rb_string;
    } else {
        rb_frozen_string = rb_str_dup(rb_string);
        rb_str_freeze(rb_frozen_string);
    }

    string = RVAL2CSTR(rb_frozen_string);
    string_len = RSTRING_LEN(rb_frozen_string);


    if (!NIL_P(rb_start_position))
        start_position = NUM2INT(rb_start_position);
    if (!NIL_P(rb_match_options))
        match_options = RVAL2GREGEXMATCHOPTIONSFLAGS(rb_match_options);

    g_regex_match_full(_SELF(self),
                       string,
                       string_len,
                       start_position,
                       match_options,
                       &match_info,
                       &error);

    if (error)
        RAISE_GERROR(error);

    if (!match_info)
        return Qnil;

    rb_match_info = GMATCHINFO2RVAL(match_info);
    g_match_info_unref(match_info);
    rb_iv_set(rb_match_info, "@string", rb_frozen_string);
    return rb_match_info;
}

static VALUE
rg_max_backref(VALUE self)
{
    return INT2NUM(g_regex_get_max_backref(_SELF(self)));
}

static VALUE
rg_capture_count(VALUE self)
{
    return INT2NUM(g_regex_get_capture_count(_SELF(self)));
}

static VALUE
rg_has_cr_or_lf_p(VALUE self)
{
    return CBOOL2RVAL(g_regex_get_has_cr_or_lf(_SELF(self)));
}

static VALUE
rg_max_lookbehind(VALUE self)
{
    return INT2NUM(g_regex_get_max_lookbehind(_SELF(self)));
}

static VALUE
rg_string_number(VALUE self, VALUE string)
{
    return INT2NUM(g_regex_get_string_number(_SELF(self), RVAL2CSTR(string)));
}

static VALUE
rg_match_all(gint argc, VALUE *argv, VALUE self)
{
    VALUE rb_string, rb_start_position, rb_match_options, rb_options;
    VALUE rb_frozen_string, rb_match_info;
    GMatchInfo *match_info = NULL;
    GError *error = NULL;
    const gchar *string;
    gssize string_len = -1;
    gint start_position = 0;
    GRegexMatchFlags match_options = 0;

    rb_scan_args(argc, argv, "11", &rb_string, &rb_options);

    rbg_scan_options(rb_options,
                     "start_position", &rb_start_position,
                     "match_options", &rb_match_options,
                     NULL);

    if (OBJ_FROZEN(rb_string)) {
        rb_frozen_string = rb_string;
    } else {
        rb_frozen_string = rb_str_dup(rb_string);
        rb_str_freeze(rb_frozen_string);
    }

    string = RVAL2CSTR(rb_frozen_string);
    string_len = RSTRING_LEN(rb_frozen_string);


    if (!NIL_P(rb_start_position))
        start_position = NUM2INT(rb_start_position);
    if (!NIL_P(rb_match_options))
        match_options = RVAL2GREGEXMATCHOPTIONSFLAGS(rb_match_options);

    g_regex_match_all_full(_SELF(self),
                           string,
                           string_len,
                           start_position,
                           match_options,
                           &match_info,
                           &error);

    if (error)
        RAISE_GERROR(error);

    if (!match_info)
        return Qnil;

    rb_match_info = GMATCHINFO2RVAL(match_info);
    g_match_info_unref(match_info);
    rb_iv_set(rb_match_info, "@string", rb_frozen_string);
    return rb_match_info;
}

static gboolean
rg_regex_eval_callback(const GMatchInfo *match_info, GString *result, gpointer user_data)
{
    VALUE cb_match_info, cb_result, callback, returned_data;
    gboolean stop_replacement =TRUE;
    cb_match_info = BOXED2RVAL(match_info, G_TYPE_MATCH_INFO);
    cb_result = CSTR2RVAL(result->str);
    callback = (VALUE) user_data;

    returned_data = rb_funcall(callback, rb_intern("call"), 2, cb_match_info, cb_result);

    /*  User can return in its callback:
     *  a string, the continue replacement is assumed
     *
     *  an array [string, Qnil]
     *
     *  any others value are ignored
     *
     * */
    if (TYPE(returned_data) == T_STRING) {
        g_string_overwrite(result, 0, RVAL2CSTR(returned_data));
    } else {
        if (TYPE(returned_data) == T_ARRAY) {
            VALUE string = rb_ary_entry(returned_data, 0);
            if (TYPE(string) == T_STRING)
                g_string_overwrite(result, 0, RVAL2CSTR(string));

            if (rb_ary_entry(returned_data, 1) == Qfalse)
                stop_replacement = FALSE;
        }
    }

    return stop_replacement;
}

static VALUE
rg_replace(gint argc, VALUE *argv, VALUE self)
{
    VALUE rb_string;
    VALUE rb_replacement;
    VALUE rb_options;
    VALUE rb_start_position;
    VALUE rb_match_options;
    VALUE rb_literal;
    VALUE callback;
    GError *error = NULL;
    gchar *modified_string;
    const gchar *string;
    const gchar *replacement;
    gssize string_len = -1;
    gint start_position = 0;
    GRegexMatchFlags match_options = 0;


    if (rb_block_given_p() == 0) {
        rb_scan_args(argc, argv, "21", &rb_string, &rb_replacement, &rb_options);

        rbg_scan_options(rb_options,
                         "start_position", &rb_start_position,
                         "match_options", &rb_match_options,
                         "literal", &rb_literal,
                         NULL);

        string = RVAL2CSTR(rb_string);
        string_len = RSTRING_LEN(rb_string);
        replacement = RVAL2CSTR(rb_replacement);

        if (!NIL_P(rb_start_position))
            start_position = NUM2INT(rb_start_position);
        if (!NIL_P(rb_match_options))
            match_options = RVAL2GREGEXMATCHOPTIONSFLAGS(rb_match_options);

        if (RVAL2CBOOL(rb_literal)) {
            modified_string = g_regex_replace_literal(_SELF(self),
                                                      string,
                                                      string_len,
                                                      start_position,
                                                      replacement,
                                                      match_options,
                                                      &error);

        } else {
            modified_string = g_regex_replace(_SELF(self),
                                              string,
                                              string_len,
                                              start_position,
                                              replacement,
                                              match_options,
                                              &error);
        }
    } else {
        rb_scan_args(argc, argv, "11", &rb_string, &rb_options);
        rbg_scan_options(rb_options,
                         "start_position", &rb_start_position,
                         "match_options", &rb_match_options,
                         NULL);

        string = RVAL2CSTR(rb_string);
        string_len = RSTRING_LEN(rb_string);

        if (!NIL_P(rb_start_position))
            start_position = NUM2INT(rb_start_position);
        if (!NIL_P(rb_match_options))
            match_options = RVAL2GREGEXMATCHOPTIONSFLAGS(rb_match_options);

        callback = rb_block_proc();

        modified_string = g_regex_replace_eval(_SELF(self),
                                      string,
                                      string_len,
                                      start_position,
                                      match_options,
                                      rg_regex_eval_callback,
                                      (gpointer) callback,
                                      &error);
    }

    if (error)
        RAISE_GERROR(error);

    return CSTR2RVAL_FREE(modified_string);
}


static VALUE
rg_s_escape_string(G_GNUC_UNUSED VALUE self, VALUE string)
{
    return CSTR2RVAL(g_regex_escape_string(RVAL2CSTR(string), RSTRING_LEN(string)));
}

static VALUE
rg_s_check_replacement(G_GNUC_UNUSED VALUE self, VALUE rb_replacement)
{
    const gchar *replacement;
    GError *error = NULL;

    replacement = RVAL2CSTR(rb_replacement);
    g_regex_check_replacement(replacement, NULL, &error);
    if (error)
        RAISE_GERROR(error);

    return Qtrue;
}

static VALUE
rg_s_have_reference_p(G_GNUC_UNUSED VALUE self, VALUE rb_replacement)
{
    const gchar *replacement;
    gboolean has_references;
    GError *error = NULL;

    replacement = RVAL2CSTR(rb_replacement);
    g_regex_check_replacement(replacement, &has_references, &error);

    if (error)
        RAISE_GERROR(error);

    return CBOOL2RVAL(has_references);
}

void
Init_glib_regex(void)
{
    VALUE RG_TARGET_NAMESPACE = G_DEF_CLASS(G_TYPE_REGEX, "Regex", mGLib);

    RG_DEF_METHOD(initialize, -1);
    RG_DEF_METHOD(pattern, 0);
    RG_DEF_METHOD(compile_flags, 0);
    RG_DEF_METHOD(match_flags, 0);
    RG_DEF_METHOD(split, -1);
    RG_DEF_METHOD(match, -1);
    RG_DEF_METHOD(max_backref, 0);
    RG_DEF_METHOD(capture_count, 0);
    RG_DEF_METHOD_P(has_cr_or_lf, 0);
    RG_DEF_METHOD(max_lookbehind, 0);
    RG_DEF_METHOD(string_number, 1);
    RG_DEF_METHOD(match_all, -1);
    RG_DEF_METHOD(replace, -1);

    RG_DEF_SMETHOD(escape_string, 1);
    RG_DEF_SMETHOD(check_replacement, 1);
    RG_DEF_SMETHOD_P(have_reference, 1);

    G_DEF_CLASS(G_TYPE_REGEX_MATCH_FLAGS, "RegexMatchFlags", mGLib);
    G_DEF_CLASS(G_TYPE_REGEX_COMPILE_FLAGS, "RegexCompileFlags", mGLib);
}

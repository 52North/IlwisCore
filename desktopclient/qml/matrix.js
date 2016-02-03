/**
 *
 * Copy of the deleted repository epistemex/transformation-matrix-js.
 * All rights are reserved to Epistemex.
 *
 * 2D transformation matrix object initialized with identity matrix.
 *
 * The matrix can synchronize a canvas context by supplying the context
 * as an argument, or later apply current absolute transform to an
 * existing context.
 *
 * All values are handled as floating point values.
 *
 * @param {CanvasRenderingContext2D} [context] - Optional context to sync with Matrix
 * @prop {number} a - scale x
 * @prop {number} b - shear y
 * @prop {number} c - shear x
 * @prop {number} d - scale y
 * @prop {number} e - translate x
 * @prop {number} f - translate y
 * @prop {CanvasRenderingContext2D|null} [context=null] - set or get current canvas context
 * @constructor
 */
function Matrix(context) {

    var me = this;
    me._t = me.transform;

    me.a = me.d = 1;
    me.b = me.c = me.e = me.f = 0;

    me.context = context;

    // reset canvas transformations (if any) to enable 100% sync.
    if (context) context.setTransform(1, 0, 0, 1, 0, 0);
}

Matrix.prototype = {

    /**
     * Concatenates transforms of this matrix onto the given child matrix and
     * returns a new matrix. This instance is used on left side.
     *
     * @param {Matrix} cm - child matrix to apply concatenation to
     * @returns {Matrix}
     */
    concat: function (cm) {
        return this.clone()._t(cm.a, cm.b, cm.c, cm.d, cm.e, cm.f);
    },

    /**
     * Flips the horizontal values.
     */
    flipX: function () {
        return this._t(-1, 0, 0, 1, 0, 0);
    },

    /**
     * Flips the vertical values.
     */
    flipY: function () {
        return this._t(1, 0, 0, -1, 0, 0);
    },

    /**
     * Reflects incoming (velocity) vector on the normal which will be the
     * current transformed x axis. Call when a trigger condition is met.
     *
     * NOTE: BETA, simple implementation
     *
     * @param {number} x - vector end point for x (start = 0)
     * @param {number} y - vector end point for y (start = 0)
     * @returns {{x: number, y: number}}
     */
    reflectVector: function (x, y) {

        var v = this.applyToPoint(0, 1),
            d = 2 * (v.x * x + v.y * y);

        x -= d * v.x;
        y -= d * v.y;

        return {x: x, y: y};
    },

    /**
     * Short-hand to reset current matrix to an identity matrix.
     */
    reset: function () {
        return this.setTransform(1, 0, 0, 1, 0, 0);
    },

    /**
     * Rotates current matrix accumulative by angle.
     * @param {number} angle - angle in radians
     */
    rotate: function (angle) {
        var cos = Math.cos(angle),
            sin = Math.sin(angle);
        return this._t(cos, sin, -sin, cos, 0, 0);
    },

    /**
     * Converts a vector given as x and y to angle, and
     * rotates (accumulative).
     * @param x
     * @param y
     * @returns {*}
     */
    rotateFromVector: function (x, y) {
        return this.rotate(Math.atan2(y, x));
    },

    /**
     * Helper method to make a rotation based on an angle in degrees.
     * @param {number} angle - angle in degrees
     */
    rotateDeg: function (angle) {
        return this.rotate(angle * Math.PI / 180);
    },

    /**
     * Scales current matrix uniformly and accumulative.
     * @param {number} f - scale factor for both x and y (1 does nothing)
     */
    scaleU: function (f) {
        return this._t(f, 0, 0, f, 0, 0);
    },

    /**
     * Scales current matrix accumulative.
     * @param {number} sx - scale factor x (1 does nothing)
     * @param {number} sy - scale factor y (1 does nothing)
     */
    scale: function (sx, sy) {
        return this._t(sx, 0, 0, sy, 0, 0);
    },

    /**
     * Scales current matrix on x axis accumulative.
     * @param {number} sx - scale factor x (1 does nothing)
     */
    scaleX: function (sx) {
        return this._t(sx, 0, 0, 1, 0, 0);
    },

    /**
     * Scales current matrix on y axis accumulative.
     * @param {number} sy - scale factor y (1 does nothing)
     */
    scaleY: function (sy) {
        return this._t(1, 0, 0, sy, 0, 0);
    },

    /**
     * Apply shear to the current matrix accumulative.
     * @param {number} sx - amount of shear for x
     * @param {number} sy - amount of shear for y
     */
    shear: function (sx, sy) {
        return this._t(1, sy, sx, 1, 0, 0);
    },

    /**
     * Apply shear for x to the current matrix accumulative.
     * @param {number} sx - amount of shear for x
     */
    shearX: function (sx) {
        return this._t(1, 0, sx, 1, 0, 0);
    },

    /**
     * Apply shear for y to the current matrix accumulative.
     * @param {number} sy - amount of shear for y
     */
    shearY: function (sy) {
        return this._t(1, sy, 0, 1, 0, 0);
    },

    /**
     * Apply skew to the current matrix accumulative.
     * @param {number} ax - angle of skew for x
     * @param {number} ay - angle of skew for y
     */
    skew: function (ax, ay) {
        return this.shear(Math.tan(ax), Math.tan(ay));
    },

    /**
     * Apply skew for x to the current matrix accumulative.
     * @param {number} ax - angle of skew for x
     */
    skewX: function (ax) {
        return this.shearX(Math.tan(ax));
    },

    /**
     * Apply skew for y to the current matrix accumulative.
     * @param {number} ay - angle of skew for y
     */
    skewY: function (ay) {
        return this.shearY(Math.tan(ay));
    },

    /**
     * Set current matrix to new absolute matrix.
     * @param {number} a - scale x
     * @param {number} b - shear y
     * @param {number} c - shear x
     * @param {number} d - scale y
     * @param {number} e - translate x
     * @param {number} f - translate y
     */
    setTransform: function (a, b, c, d, e, f) {
        var me = this;
        me.a = a;
        me.b = b;
        me.c = c;
        me.d = d;
        me.e = e;
        me.f = f;
        return me._x();
    },

    /**
     * Translate current matrix accumulative.
     * @param {number} tx - translation for x
     * @param {number} ty - translation for y
     */
    translate: function (tx, ty) {
        return this._t(1, 0, 0, 1, tx, ty);
    },

    /**
     * Translate current matrix on x axis accumulative.
     * @param {number} tx - translation for x
     */
    translateX: function (tx) {
        return this._t(1, 0, 0, 1, tx, 0);
    },

    /**
     * Translate current matrix on y axis accumulative.
     * @param {number} ty - translation for y
     */
    translateY: function (ty) {
        return this._t(1, 0, 0, 1, 0, ty);
    },

    /**
     * Multiplies current matrix with new matrix values.
     * @param {number} a2 - scale x
     * @param {number} b2 - shear y
     * @param {number} c2 - shear x
     * @param {number} d2 - scale y
     * @param {number} e2 - translate x
     * @param {number} f2 - translate y
     */
    transform: function (a2, b2, c2, d2, e2, f2) {

        var me = this,
            a1 = me.a,
            b1 = me.b,
            c1 = me.c,
            d1 = me.d,
            e1 = me.e,
            f1 = me.f;

        /* matrix order (canvas compatible):
         * ace
         * bdf
         * 001
         */
        me.a = a1 * a2 + c1 * b2;
        me.b = b1 * a2 + d1 * b2;
        me.c = a1 * c2 + c1 * d2;
        me.d = b1 * c2 + d1 * d2;
        me.e = a1 * e2 + c1 * f2 + e1;
        me.f = b1 * e2 + d1 * f2 + f1;

        return me._x();
    },

    /**
     * Divide this matrix on input matrix which must be invertible.
     * @param {Matrix} m - matrix to divide on (divisor)
     * @returns {Matrix}
     */
    divide: function (m) {

        if (!m.isInvertible())
            throw "Input matrix is not invertible";

        var im = m.inverse();

        return this._t(im.a, im.b, im.c, im.d, im.e, im.f);
    },

    /**
     * Divide current matrix on scalar value != 0.
     * @param {number} d - divisor (can not be 0)
     * @returns {Matrix}
     */
    divideScalar: function (d) {

        var me = this;
        me.a /= d;
        me.b /= d;
        me.c /= d;
        me.d /= d;
        me.e /= d;
        me.f /= d;

        return me._x();
    },

    /**
     * Get an inverse matrix of current matrix. The method returns a new
     * matrix with values you need to use to get to an identity matrix.
     * Context from parent matrix is not applied to the returned matrix.
     * @returns {Matrix}
     */
    inverse: function () {

        if (this.isIdentity()) {
            return new Matrix();
        }
        else if (!this.isInvertible()) {
            throw "Matrix is not invertible.";
        }
        else {
            var me = this,
                a = me.a,
                b = me.b,
                c = me.c,
                d = me.d,
                e = me.e,
                f = me.f,

                m = new Matrix(),
                dt = a * d - b * c;	// determinant(), skip DRY here...

            m.a = d / dt;
            m.b = -b / dt;
            m.c = -c / dt;
            m.d = a / dt;
            m.e = (c * f - d * e) / dt;
            m.f = -(a * f - b * e) / dt;

            return m;
        }
    },

    /**
     * Interpolate this matrix with another and produce a new matrix.
     * t is a value in the range [0.0, 1.0] where 0 is this instance and
     * 1 is equal to the second matrix. The t value is not constrained.
     *
     * Context from parent matrix is not applied to the returned matrix.
     *
     * Note: this interpolation is naive. For animation use the
     * intrpolateAnim() method instead.
     *
     * @param {Matrix} m2 - the matrix to interpolate with.
     * @param {number} t - interpolation [0.0, 1.0]
     * @param {CanvasRenderingContext2D} [context] - optional context to affect
     * @returns {Matrix} - new instance with the interpolated result
     */
    interpolate: function (m2, t, context) {

        var me = this,
            m = context ? new Matrix(context) : new Matrix();

        m.a = me.a + (m2.a - me.a) * t;
        m.b = me.b + (m2.b - me.b) * t;
        m.c = me.c + (m2.c - me.c) * t;
        m.d = me.d + (m2.d - me.d) * t;
        m.e = me.e + (m2.e - me.e) * t;
        m.f = me.f + (m2.f - me.f) * t;

        return m._x();
    },

    /**
     * Interpolate this matrix with another and produce a new matrix.
     * t is a value in the range [0.0, 1.0] where 0 is this instance and
     * 1 is equal to the second matrix. The t value is not constrained.
     *
     * Context from parent matrix is not applied to the returned matrix.
     *
     * Note: this interpolation method uses decomposition which makes
     * it suitable for animations (in particular where rotation takes
     * places).
     *
     * @param {Matrix} m2 - the matrix to interpolate with.
     * @param {number} t - interpolation [0.0, 1.0]
     * @param {CanvasRenderingContext2D} [context] - optional context to affect
     * @returns {Matrix} - new instance with the interpolated result
     */
    interpolateAnim: function (m2, t, context) {

        var me = this,
            m = context ? new Matrix(context) : new Matrix(),
            d1 = me.decompose(),
            d2 = m2.decompose(),
            rotation = d1.rotation + (d2.rotation - d1.rotation) * t,
            translateX = d1.translate.x + (d2.translate.x - d1.translate.x) * t,
            translateY = d1.translate.y + (d2.translate.y - d1.translate.y) * t,
            scaleX = d1.scale.x + (d2.scale.x - d1.scale.x) * t,
            scaleY = d1.scale.y + (d2.scale.y - d1.scale.y) * t
            ;

        m.translate(translateX, translateY);
        m.rotate(rotation);
        m.scale(scaleX, scaleY);

        return m._x();
    },

    /**
     * Decompose the current matrix into simple transforms using either
     * QR (default) or LU decomposition. Code adapted from
     * http://www.maths-informatique-jeux.com/blog/frederic/?post/2013/12/01/Decomposition-of-2D-transform-matrices
     *
     * The result must be applied in the following order to reproduce the current matrix:
     *
     *     QR: translate -> rotate -> scale -> skewX
     *     LU: translate -> skewY  -> scale -> skewX
     *
     * @param {boolean} [useLU=false] - set to true to use LU rather than QR algorithm
     * @returns {*} - an object containing current decomposed values (rotate, skew, scale, translate)
     */
    decompose: function (useLU) {

        var me = this,
            a = me.a,
            b = me.b,
            c = me.c,
            d = me.d,
            acos = Math.acos,
            atan = Math.atan,
            sqrt = Math.sqrt,
            pi = Math.PI,

            translate = {x: me.e, y: me.f},
            rotation = 0,
            scale = {x: 1, y: 1},
            skew = {x: 0, y: 0},

            determ = a * d - b * c;	// determinant(), skip DRY here...

        if (useLU) {
            if (a) {
                skew = {x: atan(c / a), y: atan(b / a)};
                scale = {x: a, y: determ / a};
            }
            else if (b) {
                rotation = pi * 0.5;
                scale = {x: b, y: determ / b};
                skew.x = atan(d / b);
            }
            else { // a = b = 0
                scale = {x: c, y: d};
                skew.x = pi * 0.25;
            }
        }
        else {
            // Apply the QR-like decomposition.
            if (a || b) {
                var r = sqrt(a * a + b * b);
                rotation = b > 0 ? acos(a / r) : -acos(a / r);
                scale = {x: r, y: determ / r};
                skew.x = atan((a * c + b * d) / (r * r));
            }
            else if (c || d) {
                var s = sqrt(c * c + d * d);
                rotation = pi * 0.5 - (d > 0 ? acos(-c / s) : -acos(c / s));
                scale = {x: determ / s, y: s};
                skew.y = atan((a * c + b * d) / (s * s));
            }
            else { // a = b = c = d = 0
                scale = {x: 0, y: 0};		// = invalid matrix
            }
        }

        return {
            scale: scale,
            translate: translate,
            rotation: rotation,
            skew: skew
        };
    },

    /**
     * Returns the determinant of the current matrix.
     * @returns {number}
     */
    determinant: function () {
        return this.a * this.d - this.b * this.c;
    },

    /**
     * Apply current matrix to x and y point.
     * Returns a point object.
     *
     * @param {number} x - value for x
     * @param {number} y - value for y
     * @returns {{x: number, y: number}} A new transformed point object
     */
    applyToPoint: function (x, y) {

        var me = this;

        return {
            x: x * me.a + y * me.c + me.e,
            y: x * me.b + y * me.d + me.f
        };
    },

    /**
     * Apply current matrix to array with point objects or point pairs.
     * Returns a new array with points in the same format as the input array.
     *
     * A point object is an object literal:
     *
     * {x: x, y: y}
     *
     * so an array would contain either:
     *
     * [{x: x1, y: y1}, {x: x2, y: y2}, ... {x: xn, y: yn}]
     *
     * or
     * [x1, y1, x2, y2, ... xn, yn]
     *
     * @param {Array} points - array with point objects or pairs
     * @returns {Array} A new array with transformed points
     */
    applyToArray: function (points) {

        var i = 0, p, l,
            mxPoints = [];

        if (typeof points[0] === 'number') {

            l = points.length;

            while (i < l) {
                p = this.applyToPoint(points[i++], points[i++]);
                mxPoints.push(p.x, p.y);
            }
        }
        else {
            for (; p = points[i]; i++) {
                mxPoints.push(this.applyToPoint(p.x, p.y));
            }
        }

        return mxPoints;
    },

    /**
     * Apply current matrix to a typed array with point pairs. Although
     * the input array may be an ordinary array, this method is intended
     * for more performant use where typed arrays are used. The returned
     * array is regardless always returned as a Float32Array.
     *
     * @param {*} points - (typed) array with point pairs
     * @param {boolean} [use64=false] - use Float64Array instead of Float32Array
     * @returns {*} A new typed array with transformed points
     */
    applyToTypedArray: function (points, use64) {

        var i = 0, p,
            l = points.length,
            mxPoints = use64 ? new Float64Array(l) : new Float32Array(l);

        while (i < l) {
            p = this.applyToPoint(points[i], points[i + 1]);
            mxPoints[i++] = p.x;
            mxPoints[i++] = p.y;
        }

        return mxPoints;
    },

    /**
     * Apply to any canvas 2D context object. This does not affect the
     * context that optionally was referenced in constructor unless it is
     * the same context.
     * @param {CanvasRenderingContext2D} context
     */
    applyToContext: function (context) {
        var me = this;
        context.setTransform(me.a, me.b, me.c, me.d, me.e, me.f);
        return me;
    },

    /**
     * Returns true if matrix is an identity matrix (no transforms applied).
     * @returns {boolean} True if identity (not transformed)
     */
    isIdentity: function () {
        var me = this;
        return (me._q(me.a, 1) &&
        me._q(me.b, 0) &&
        me._q(me.c, 0) &&
        me._q(me.d, 1) &&
        me._q(me.e, 0) &&
        me._q(me.f, 0));
    },

    /**
     * Returns true if matrix is invertible
     * @returns {boolean}
     */
    isInvertible: function () {
        return !this._q(this.determinant(), 0)
    },

    /**
     * Test if matrix is valid.
     */
    isValid: function () {
        return !this._q(this.a * this.d, 0);
    },

    /**
     * Clones current instance and returning a new matrix.
     * @param {boolean} [noContext=false] don't clone context reference if true
     * @returns {Matrix}
     */
    clone: function (noContext) {
        var me = this,
            m = new Matrix();
        m.a = me.a;
        m.b = me.b;
        m.c = me.c;
        m.d = me.d;
        m.e = me.e;
        m.f = me.f;
        if (!noContext) m.context = me.context;

        return m;
    },

    /**
     * Compares current matrix with another matrix. Returns true if equal
     * (within epsilon tolerance).
     * @param {Matrix} m - matrix to compare this matrix with
     * @returns {boolean}
     */
    isEqual: function (m) {

        var me = this,
            q = me._q;

        return (q(me.a, m.a) &&
        q(me.b, m.b) &&
        q(me.c, m.c) &&
        q(me.d, m.d) &&
        q(me.e, m.e) &&
        q(me.f, m.f));
    },

    /**
     * Returns an array with current matrix values.
     * @returns {Array}
     */
    toArray: function () {
        var me = this;
        return [me.a, me.b, me.c, me.d, me.e, me.f];
    },

    /**
     * Generates a matrix() string that can be used with CSS transform.
     * @returns {string}
     */
    toCSS: function () {
        return "matrix(" + this.toArray() + ")";
    },

    /**
     * Generates a matrix3d() string that can be used with CSS transform.
     * @returns {string}
     */
    toCSS3D: function () {
        var me = this;
        return "matrix3d(" + me.a + ", " + me.b + ", 0, 0, " + me.c + ", " + me.d + ", 0, 0, 0, 0, 1, 0, " + me.e + ", " + me.f + ", 0, 1)";
    },

    /**
     * Returns a JSON compatible string of current matrix.
     * @returns {string}
     */
    toJSON: function () {
        return JSON.stringify(this.toArray());
    },

    /**
     * Returns a string with current matrix as comma-separated list.
     * @returns {string}
     */
    toString: function () {
        return "" + this.toArray();
    },

    /**
     * Compares floating point values with some tolerance (epsilon)
     * @param {number} f1 - float 1
     * @param {number} f2 - float 2
     * @returns {boolean}
     * @private
     */
    _q: function (f1, f2) {
        return Math.abs(f1 - f2) < 1e-14;
    },

    /**
     * Apply current absolute matrix to context if defined, to sync it.
     * @private
     */
    _x: function () {
        var me = this;
        if (me.context)
            me.context.setTransform(me.a, me.b, me.c, me.d, me.e, me.f);
        return me;
    }
};

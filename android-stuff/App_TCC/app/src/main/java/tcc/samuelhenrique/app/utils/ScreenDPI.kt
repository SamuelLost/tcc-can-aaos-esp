package tcc.samuelhenrique.app.utils

import android.content.Context

object ScreenDPI {
    fun getScreenDPI(context: Context): Int {
        val metrics = context.resources.displayMetrics
        return metrics.densityDpi
    }
}
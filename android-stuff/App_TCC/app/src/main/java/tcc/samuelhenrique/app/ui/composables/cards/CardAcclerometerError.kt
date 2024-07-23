package tcc.samuelhenrique.app.ui.composables.cards

import android.util.Log
import androidx.compose.runtime.Composable
import tcc.samuelhenrique.app.ui.composables.CurrentReading
import tcc.samuelhenrique.app.ui.composables.Description
import tcc.samuelhenrique.app.ui.composables.ExpandableCard
import tcc.samuelhenrique.app.ui.composables.Gravity
import java.util.Locale
import kotlin.math.PI
import kotlin.math.atan
import kotlin.math.atan2
import kotlin.math.pow
import kotlin.math.sqrt

@Composable
fun CardAccelerometerError(axis: IntArray, accFaultCode: String) {
    ExpandableCard(titleCard = if (accFaultCode == "ACC-E1") "Defeito no sensor de aceleração" else "Defeito no sensor de aceleração",
        description = { Description("Defeito na leitura do sensor de aceleração") },
        gravity = { Gravity("Média") },
        // Imprimir o valor do sensor de aceleração como X: 0.0, Y: 0.0, Z: 0.0 usando joinToString
        currentReading = {
            CurrentReading(
                "%.1f º".format(
                    locale = Locale("pt", "BR"),
                    processAcceleration(axis)
                )
            )
        }
    )

}

fun processAccelerometerData(acc: IntArray): Double {
    // Calcular angulação do veículo em relação ao solo usando aceleração (roll)
    var x = acc[0].toDouble()
    var y = acc[1].toDouble()
    var z = acc[2].toDouble()

    if (y == 0.0 && z == 0.0) {
        return 0.0
    }
    var roll = atan(
        y / sqrt(
            x.pow(2) + z.pow(2)
        )
    ) * 180 / PI

    Log.d("MainActivity", "Roll: $roll")
    return roll
}

fun processAcceleration(acc: IntArray): Double {
    var ax = acc[0].toDouble()
    var ay = acc[1].toDouble()
    var az = acc[2].toDouble()

    if (ay == 0.0 && az == 0.0) {
        return 0.0
    }

    var anorm = sqrt(ax.pow(2) + ay.pow(2) + az.pow(2))
    var axbar = ax / anorm
    var aybar = ay / anorm
    var azbar = az / anorm

    var roll = atan2(aybar, sqrt(azbar.pow(2) + axbar.pow(2))) * 180 / PI

    Log.i("MainActivity", "Roll: $roll")

    return roll
}
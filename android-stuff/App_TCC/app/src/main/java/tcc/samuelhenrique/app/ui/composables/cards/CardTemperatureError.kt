package tcc.samuelhenrique.app.ui.composables.cards

import androidx.compose.runtime.Composable
import tcc.samuelhenrique.app.ui.composables.CurrentReading
import tcc.samuelhenrique.app.ui.composables.Description
import tcc.samuelhenrique.app.ui.composables.ExpandableCard
import tcc.samuelhenrique.app.ui.composables.Gravity
import java.util.Locale

@Composable
fun CardTemperatureError(temperature: Float, temperatureFaultCode: String) {
    ExpandableCard(
        titleCard = if (temperatureFaultCode == "TMP-E1") "Defeito no sensor de temperatura" else "Defeito no sensor de temperatura",
        description = { Description("Defeito na leitura do sensor de temperatura. Pare o carro imediatamente e procure a oficina mais próxima!") },
        gravity = { Gravity("Alta") },
        currentReading = {
            CurrentReading(
                "%.1f ºC".format(
                    locale = Locale("pt", "BR"),
                    temperature
                )
            )
        }
    )
}
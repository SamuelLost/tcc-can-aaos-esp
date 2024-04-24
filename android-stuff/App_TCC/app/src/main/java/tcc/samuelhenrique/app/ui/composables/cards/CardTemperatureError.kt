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
        titleCard = temperatureFaultCode,
        description = { Description("Erro na leitura do sensor de temperatura") },
        gravity = { Gravity("Média") },
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